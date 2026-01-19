#include "ScriptingModule.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Resources/ScriptResource.h"
#include "Loopie/Importers/ScriptImporter.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/mono-config.h>
#include "Loopie/Importers/TextureImporter.h"

namespace Loopie {

    void ScriptingModule::OnLoad() {
        mono_set_dirs("vendor/mono/lib", "vendor/mono/etc");
        mono_config_parse(NULL);

        m_RootDomain = mono_jit_init("LoopieRootDomain");
        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
        mono_domain_set(m_AppDomain, true);
    }

    void ScriptingModule::OnUpdate() {
        m_WatcherTimer += 0.016f;

        if (m_WatcherTimer >= m_WatcherInterval) {
            CheckForScriptChanges();
            m_WatcherTimer = 0.0f;
        }
    }

    void ScriptingModule::CheckForScriptChanges() {
        std::string scriptsPath = R"(C:\Users\paugo\Documents\GitHub\Engine\G1_LoopieEngine_Scripting\Projects\Scripting Project\Assets\Scenes\a\Assets\Scripts)";

        if (!std::filesystem::exists(scriptsPath)) {
            Log::Error("ERROR: La ruta no existe. Revisa si has movido la carpeta: {0}", scriptsPath);
            return;
        }

        Log::Info("--- Escaneando Scripts ---");

        if (std::filesystem::is_empty(scriptsPath)) {
            Log::Error("AVISO: La carpeta existe pero está totalmente vacía.");
        }

        for (auto& entry : std::filesystem::recursive_directory_iterator(scriptsPath)) {
            std::string fileName = entry.path().filename().string();
            std::string extension = entry.path().extension().string();

            Log::Info("Archivo visto en disco: {0} (Ext: {1})", fileName, extension);

            if (extension == ".cs" || extension == ".CS") {
                std::string pathStr = entry.path().string();
                auto currentWriteTime = std::filesystem::last_write_time(entry.path());

                auto it = m_FileWatchMap.find(pathStr);

                if (it == m_FileWatchMap.end()) {
                    Log::Info("-> Detectado como [NUEVO]: {0}", fileName);
                    TextureImporter::Import(pathStr);
                    m_FileWatchMap[pathStr] = currentWriteTime;
                }
                else if (currentWriteTime > it->second) {
                    Log::Info("-> Detectado como [MODIFICADO]: {0}", fileName);
                    TextureImporter::Import(pathStr);
                    m_FileWatchMap[pathStr] = currentWriteTime;
                }
            }
        }

        Log::Info("--- Fin del Escaneo ---");
    }

    MonoAssembly* ScriptingModule::LoadAssembly(const std::string& filePath) {
        MonoAssembly* assembly = mono_domain_assembly_open(m_AppDomain, filePath.c_str());
        if (assembly) {
            m_AssemblyImage = mono_assembly_get_image(assembly);
        }
        return assembly;
    }

    void ScriptingModule::OnUnload() {
        if (m_RootDomain) {
            mono_jit_cleanup(m_RootDomain);
        }
    }

}