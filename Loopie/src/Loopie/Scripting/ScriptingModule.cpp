#include "ScriptingModule.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include "Loopie/Components/ScriptComponent.h"
#include "ScriptGlue.h"
#include <mono/metadata/mono-config.h>
#include <filesystem>

MonoImage* g_GameAssemblyImage = nullptr;
namespace fs = std::filesystem;

namespace Loopie {

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
            mono_domain_set(mono_get_root_domain(), false);
        }

        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
        mono_domain_set(m_AppDomain, true);

        std::string scriptDllPath = "C:/Users/paugo/Documents/GitHub/Engine/G1_LoopieEngine_Scripting/Assets/Scripts/Loopie.Core.dll";

        if (fs::exists(scriptDllPath)) {
            MonoAssembly* coreAssembly = LoadAssembly(scriptDllPath);
            if (coreAssembly) {
                m_AssemblyImage = mono_assembly_get_image(coreAssembly);
                g_GameAssemblyImage = m_AssemblyImage;
                ScriptGlue::RegisterGlue();
                Log::Info("Scripting Core cargado: {0}", scriptDllPath);
            }
        }

        // Reemplaza el bucle que da error por este
        const MonoTableInfo* typeTable = mono_image_get_table_info(g_GameAssemblyImage, MONO_TABLE_TYPEDEF);
        int numTypes = mono_table_info_get_rows(typeTable);

        Log::Info("--- Clases detectadas en la DLL ---");
        for (int i = 0; i < numTypes; i++) {
            // mono_class_get requiere la imagen Y el token
            // Usamos el desplazamiento manual del token que es mas seguro
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* name = mono_metadata_string_heap(g_GameAssemblyImage, cols[MONO_TYPEDEF_NAME]);
            const char* ns = mono_metadata_string_heap(g_GameAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);

            Log::Info("Found: {0}.{1}", ns, name);
        }
        Log::Info("----------------------------------");
    }

    void ScriptingModule::ReloadAssembly() {
        Log::Debug("Recargando scripts...");

        LoadCoreAssembly();

        Scene* activeScene = Application::GetInstance().m_scene;
        if (activeScene) {
            for (auto& [uuid, entity] : activeScene->GetAllEntities()) {
                if (auto* script = entity->GetComponent<ScriptComponent>()) {
                    script->SetScript(script->GetScriptName());
                }
            }
        }
        Log::Debug("Sincronizacion de scripts completada.");
    }

    void ScriptingModule::CheckForScriptChanges() {
        std::string scriptsPath = R"(C:\Users\paugo\Documents\GitHub\Engine\G1_LoopieEngine_Scripting\Projects\Scripting Project\Assets\Scenes\a\Assets\Scripts)";

        if (!fs::exists(scriptsPath)) return;

        bool needsReload = false;
        for (auto& entry : fs::recursive_directory_iterator(scriptsPath)) {
            if (entry.path().extension() == ".cs" || entry.path().extension() == ".CS") {
                std::string pathStr = entry.path().string();
                auto currentWriteTime = fs::last_write_time(entry.path());

                if (m_FileWatchMap.find(pathStr) == m_FileWatchMap.end()) {
                    m_FileWatchMap[pathStr] = currentWriteTime;
                    needsReload = true;
                }
                else if (currentWriteTime > m_FileWatchMap[pathStr]) {
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
        m_WatcherTimer += 0.016f;
        if (m_WatcherTimer >= m_WatcherInterval) {
            CheckForScriptChanges();
            m_WatcherTimer = 0.0f;
        }
    }

    MonoAssembly* ScriptingModule::LoadAssembly(const std::string& filePath) {
        MonoAssembly* assembly = mono_domain_assembly_open(m_AppDomain, filePath.c_str());
        return assembly;
    }

    void ScriptingModule::OnUnload() {
        if (m_RootDomain) {
            mono_jit_cleanup(m_RootDomain);
        }
    }
}