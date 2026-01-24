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
        }
        LoadCoreAssembly();
    }

    void ScriptingModule::LoadCoreAssembly() {
        if (m_AppDomain) {
            mono_domain_set(m_RootDomain, false);
            mono_domain_unload(m_AppDomain);
        }
        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
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
    }

    void ScriptingModule::InitializeScriptInstance(MonoObject* instance, const std::string& uuid) {
        if (!instance) {
            Log::Error("InitializeScriptInstance: Instancia nula!");
            return;
        }

        MonoClass* monoClass = mono_object_get_class(instance);

        // 1. Buscamos el campo en la clase base LoopieScript
        // Importante: No usamos propiedades, buscamos el FIELD "EntityID"
        MonoClassField* field = mono_class_get_field_from_name(monoClass, "EntityID");

        if (!field) {
            // Si el script hereda de LoopieScript, el campo esta en el padre
            MonoClass* parentClass = mono_class_get_parent(monoClass);
            if (parentClass) {
                field = mono_class_get_field_from_name(parentClass, "EntityID");
            }
        }

        if (field) {
            MonoString* monoStr = mono_string_new(m_AppDomain, uuid.c_str());
            mono_field_set_value(instance, field, monoStr);
            Log::Info("UUID '{0}' inyectado correctamente en C#", uuid);
        }
        else {
            Log::Error("ERROR CRITICO: No se encontro el campo 'EntityID' en C#. ¿Has compilado el paso 1?");
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

    void ScriptingModule::CheckForScriptChanges() {
        std::string scriptsPath = R"(../../../LoopieScriptCore)";
        if (!fs::exists(scriptsPath)) return;
        bool needsReload = false;
        for (auto& entry : fs::recursive_directory_iterator(scriptsPath)) {
            if (entry.path().extension() == ".cs") {
                auto currentWriteTime = fs::last_write_time(entry.path());
                if (m_FileWatchMap[entry.path().string()] < currentWriteTime) {
                    m_FileWatchMap[entry.path().string()] = currentWriteTime;
                    needsReload = true;
                }
            }
        }
        if (needsReload) ReloadAssembly();
    }

    void ScriptingModule::OnUpdate() {
        static float timer = 0;
        timer += 0.016f;
        if (timer >= 1.0f) { CheckForScriptChanges(); timer = 0; }
    }

    void ScriptingModule::OnUnload() {
        if (m_AppDomain) { mono_domain_set(m_RootDomain, false); mono_domain_unload(m_AppDomain); }
        if (m_RootDomain) { mono_jit_cleanup(m_RootDomain); }
    }

    void ScriptingModule::CreateScriptAsset(const std::filesystem::path& directory, const std::string& name) {
        std::string content = "using System;\nusing Loopie;\n\npublic class " + name + " : LoopieScript\n{\n    public override void Start() { }\n    public override void Update(float dt) { }\n}\n";
        std::ofstream f1(directory / (name + ".cs")); f1 << content; f1.close();
        std::ofstream f2(fs::path("../../../LoopieScriptCore/") / (name + ".cs")); f2 << content; f2.close();
    }

    std::vector<std::string> ScriptingModule::GetAvailableScripts() {
        std::vector<std::string> scripts;
        for (auto& entry : fs::directory_iterator("../../../Assets/Scripts")) {
            if (entry.path().extension() == ".cs") scripts.push_back(entry.path().stem().string());
        }
        return scripts;
    }
}