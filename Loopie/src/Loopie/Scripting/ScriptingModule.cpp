#include "ScriptingModule.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include "Loopie/Components/ScriptComponent.h"
#include "ScriptGlue.h"
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>
#include <filesystem>
#include <fstream>

MonoImage* g_GameAssemblyImage = nullptr;
namespace fs = std::filesystem;

namespace Loopie {

    ScriptingModule* ScriptingModule::s_Instance = nullptr;
    MonoDomain* ScriptingModule::m_RootDomain = nullptr;
    MonoDomain* ScriptingModule::m_AppDomain = nullptr;
    MonoImage* ScriptingModule::m_AssemblyImage = nullptr;
    MonoAssembly* ScriptingModule::m_CoreAssembly = nullptr;

    std::unordered_map<std::string, std::filesystem::file_time_type> ScriptingModule::m_FileWatchMap{};

    void ScriptingModule::OnLoad() {
        if (!m_RootDomain) {
            fs::path rootPath = fs::current_path();
            fs::path libPath = (rootPath / "vendor" / "mono" / "lib").make_preferred();
            fs::path etcPath = (rootPath / "vendor" / "mono" / "etc").make_preferred();

            mono_set_dirs(libPath.string().c_str(), etcPath.string().c_str());
            mono_config_parse(NULL);

            m_RootDomain = mono_jit_init("LoopieRootDomain");

            if (!m_RootDomain) {
                Log::Error("No se pudo inicializar Mono JIT!");
                return;
            }
        }
        LoadCoreAssembly();
    }

    void ScriptingModule::LoadCoreAssembly() {
        if (m_AppDomain) {
            mono_domain_set(m_RootDomain, false);
            mono_domain_unload(m_AppDomain);
            m_AppDomain = nullptr;
            m_AssemblyImage = nullptr;
            m_CoreAssembly = nullptr;
            g_GameAssemblyImage = nullptr;
        }

        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
        if (!m_AppDomain) return;

        mono_domain_set(m_AppDomain, true);

        std::string scriptDllPath = "../../../Assets/Scripts/Loopie.Core.dll";
        if (fs::exists(scriptDllPath)) {
            m_CoreAssembly = mono_domain_assembly_open(m_AppDomain, scriptDllPath.c_str());
            if (m_CoreAssembly) {
                m_AssemblyImage = mono_assembly_get_image(m_CoreAssembly);
                g_GameAssemblyImage = m_AssemblyImage;
                ScriptGlue::RegisterGlue();
            }
        }
        else {
            Log::Error("No se encontro la DLL de scripts en: {0}", scriptDllPath);
        }
    }

    void ScriptingModule::InitializeScriptInstance(MonoObject* instance, const std::string& uuid) {
        if (!instance) return;

        MonoClass* monoClass = mono_object_get_class(instance);

        MonoClassField* idField = mono_class_get_field_from_name(monoClass, "EntityID");

        if (!idField) {
            MonoClass* parentClass = mono_class_get_parent(monoClass);
            if (parentClass) {
                idField = mono_class_get_field_from_name(parentClass, "EntityID");
            }
        }

        if (idField) {
            MonoString* monoStr = mono_string_new(m_AppDomain, uuid.c_str());
            mono_field_set_value(instance, idField, monoStr);
        }
        else {
            Log::Error("ERROR CRITICO: No se encontro el campo 'public string EntityID' en C#.");
        }
    }

    void ScriptingModule::ReloadAssembly() {
        LoadCoreAssembly();
        Scene* activeScene = Application::GetInstance().m_scene;
        if (activeScene) {
            for (auto& [uuid, entity] : activeScene->GetAllEntities()) {
                if (auto* script = entity->GetComponent<ScriptComponent>()) {
                    script->SetScript(script->GetScriptName());
                }
            }
        }
    }

    void ScriptingModule::SyncScriptsFromAssetsToCore() {
        std::string assetsPathStr = "../../../Assets";
        std::string corePathStr = "../../../LoopieScriptCore";

        if (!fs::exists(assetsPathStr) || !fs::exists(corePathStr)) return;

        for (auto& entry : fs::recursive_directory_iterator(assetsPathStr)) {
            if (entry.path().extension() == ".cs") {

                std::string filename = entry.path().filename().string();
                fs::path destPath = fs::path(corePathStr) / filename;

                bool shouldCopy = false;

                if (!fs::exists(destPath)) {
                    shouldCopy = true;
                }
                else {
                    auto srcTime = fs::last_write_time(entry.path());
                    auto destTime = fs::last_write_time(destPath);
                    if (srcTime > destTime) {
                        shouldCopy = true;
                    }
                }

                if (shouldCopy) {
                    try {
                        fs::copy_file(entry.path(), destPath, fs::copy_options::overwrite_existing);
                    }
                    catch (std::exception& e) {
                        Log::Error("Error sincronizando script: {0}", e.what());
                    }
                }
            }
        }
    }

    void ScriptingModule::CheckForScriptChanges() {
        SyncScriptsFromAssetsToCore();

        std::string scriptsPath = R"(../../../LoopieScriptCore)";
        if (!fs::exists(scriptsPath)) return;

        bool needsReload = false;
        for (auto& entry : fs::recursive_directory_iterator(scriptsPath)) {
            if (entry.path().extension() == ".cs") {
                std::string pathStr = entry.path().string();
                auto currentWriteTime = fs::last_write_time(entry.path());

                if (m_FileWatchMap.find(pathStr) == m_FileWatchMap.end() ||
                    currentWriteTime > m_FileWatchMap[pathStr])
                {
                    m_FileWatchMap[pathStr] = currentWriteTime;
                    needsReload = true;
                }
            }
        }

        if (needsReload) {
            ReloadAssembly();
        }
    }

    void ScriptingModule::OnUpdate() {
        static float timer = 0;
        timer += 0.016f; 
        if (timer >= m_WatcherInterval) {
            CheckForScriptChanges();
            timer = 0;
        }
    }

    MonoAssembly* ScriptingModule::LoadAssembly(const std::string& filePath) {
        return mono_domain_assembly_open(m_AppDomain, filePath.c_str());
    }

    void ScriptingModule::OnUnload() {
        if (m_AppDomain) {
            mono_domain_set(m_RootDomain, false);
            mono_domain_unload(m_AppDomain);
            m_AppDomain = nullptr;
        }
        if (m_RootDomain) {
            mono_jit_cleanup(m_RootDomain);
            m_RootDomain = nullptr;
        }
    }

    void ScriptingModule::CreateScriptAsset(const std::filesystem::path& directory, const std::string& name) {
        std::string content =
            "using System;\nusing Loopie;\n\n"
            "public class " + name + " : LoopieScript\n{\n"
            "    public override void Start()\n"
            "    {\n"
            "        InternalCalls.Log(\"Script " + name + " ready. ID: \" + EntityID);\n"
            "    }\n\n"
            "    public override void Update(float dt)\n"
            "    {\n"
            "    }\n"
            "}\n";

        std::ofstream f1(directory / (name + ".cs"));
        f1 << content;
        f1.close();

        std::ofstream f2(fs::path("../../../LoopieScriptCore/") / (name + ".cs"));
        f2 << content;
        f2.close();
    }

    std::vector<std::string> ScriptingModule::GetAvailableScripts() {
        std::vector<std::string> scripts;
        std::string path = "../../../Assets/Scripts";
        if (!fs::exists(path)) return scripts;

        for (auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".cs") {
                scripts.push_back(entry.path().stem().string());
            }
        }
        return scripts;
    }
}