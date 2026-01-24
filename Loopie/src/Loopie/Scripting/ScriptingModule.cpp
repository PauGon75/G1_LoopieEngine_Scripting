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

            Log::Info("Mono Root Domain inicializado correctamente.");
        }

        LoadCoreAssembly();
    }

    void ScriptingModule::LoadCoreAssembly() {
        // Descargar dominio anterior si existe
        if (m_AppDomain) {
            Log::Debug("Descargando AppDomain anterior...");
            mono_domain_set(m_RootDomain, false);
            mono_domain_unload(m_AppDomain);
            m_AppDomain = nullptr;
            m_AssemblyImage = nullptr;
            m_CoreAssembly = nullptr;
            g_GameAssemblyImage = nullptr;
        }

        // Crear nuevo AppDomain
        Log::Debug("Creando nuevo AppDomain...");
        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);

        if (!m_AppDomain) {
            Log::Error("No se pudo crear AppDomain!");
            return;
        }

        // Establecer el AppDomain como activo
        if (!mono_domain_set(m_AppDomain, true)) {
            Log::Error("No se pudo establecer el AppDomain como activo!");
            return;
        }

        Log::Debug("AppDomain creado y establecido: {0}", (void*)m_AppDomain);

        std::string scriptDllPath = "C:/Users/paugo/Documents/GitHub/Engine/G1_LoopieEngine_Scripting/Assets/Scripts/Loopie.Core.dll";

        if (fs::exists(scriptDllPath)) {
            Log::Debug("Cargando assembly desde: {0}", scriptDllPath);

            m_CoreAssembly = LoadAssembly(scriptDllPath);

            if (m_CoreAssembly) {
                m_AssemblyImage = mono_assembly_get_image(m_CoreAssembly);
                g_GameAssemblyImage = m_AssemblyImage;

                Log::Debug("Assembly cargado. Image: {0}", (void*)g_GameAssemblyImage);

                ScriptGlue::RegisterGlue();
                Log::Info("Scripting Core cargado: {0}", scriptDllPath);
            }
            else {
                Log::Error("No se pudo cargar el assembly!");
                return;
            }
        }
        else {
            Log::Error("No se encontro el archivo DLL: {0}", scriptDllPath);
            return;
        }

        // Listar clases detectadas
        const MonoTableInfo* typeTable = mono_image_get_table_info(g_GameAssemblyImage, MONO_TABLE_TYPEDEF);
        int numTypes = mono_table_info_get_rows(typeTable);

        Log::Info("--- Clases detectadas en la DLL ---");
        for (int i = 0; i < numTypes; i++) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* name = mono_metadata_string_heap(g_GameAssemblyImage, cols[MONO_TYPEDEF_NAME]);
            const char* ns = mono_metadata_string_heap(g_GameAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);

            if (std::string(name) != "<Module>") {
                Log::Info("Found: {0}.{1}", ns, name);
            }
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
        if (!assembly) {
            Log::Error("mono_domain_assembly_open fallo para: {0}", filePath);
        }
        return assembly;
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
}