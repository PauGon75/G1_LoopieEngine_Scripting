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
        // Usamos R"(...)" para que C++ ignore los caracteres de escape en la ruta
        std::string scriptsPath = R"(C:\Users\paugo\Documents\GitHub\Engine\G1_LoopieEngine_Scripting\Projects\Scripting Project\Assets\Scenes\a\Assets\Scripts)";

        if (!std::filesystem::exists(scriptsPath)) {
            Log::Error("Watcher: La ruta no existe: {0}", scriptsPath);
            return;
        }

        Log::Info("--- Escaneando Scripts en: {0} ---", scriptsPath);

        bool foundAny = false;
        for (auto& entry : std::filesystem::recursive_directory_iterator(scriptsPath)) {
            if (entry.path().extension() == ".cs") {
                foundAny = true;
                std::string pathStr = entry.path().string();
                std::string fileName = entry.path().filename().string();
                auto currentWriteTime = std::filesystem::last_write_time(entry.path());

                // DEBUG: Imprime cada archivo encontrado
                Log::Info("Encontrado: {0} | Ruta completa: {1}", fileName, pathStr);
				TextureImporter::Test();
                auto it = m_FileWatchMap.find(pathStr);

                if (it == m_FileWatchMap.end()) {
                    Log::Info("Estado: [NUEVO] - Procediendo a importar {0}", fileName);

                    /*auto newScript = ScriptImporter::Import(pathStr);
                    if (newScript) {
                        m_FileWatchMap[pathStr] = currentWriteTime;
                    }*/
                }
                else if (currentWriteTime > it->second) {
                    Log::Info("Estado: [MODIFICADO] - Procediendo a recompilar {0}", fileName);

                    auto resource = ResourceManager::GetResource<ScriptResource>(pathStr);
                    if (resource) {
                        if (TextureImporter::Compile(resource)) {
                            m_FileWatchMap[pathStr] = currentWriteTime;
                            resource->SetLastWriteTime(currentWriteTime);
                        }
                    }
                    /*else {
                        Log::Error("Recurso no encontrado en Manager para {0}. Re-importando...", fileName);
                        ScriptImporter::Import(pathStr);
                        m_FileWatchMap[pathStr] = currentWriteTime;
                    }*/
                }
                else {
                    Log::Info("Estado: [SIN CAMBIOS] - {0}", fileName);
                }
            }
        }

        if (!foundAny) {
            Log::Error("No se encontraron archivos .cs en la carpeta.");
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