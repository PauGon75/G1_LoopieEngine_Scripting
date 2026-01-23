#include "ScriptingModule.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Resources/ScriptResource.h"
#include "Loopie/Importers/ScriptImporter.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/mono-config.h>
#include "Loopie/Importers/TextureImporter.h"
#include "ScriptGlue.h"


MonoImage* g_GameAssemblyImage = nullptr;

namespace Loopie {

#include <filesystem>
    namespace fs = std::filesystem;
    void ScriptingModule::OnLoad() {

        fs::path rootPath = fs::current_path();

        fs::path libPath = (rootPath / "vendor" / "mono" / "lib").make_preferred();
        fs::path etcPath = (rootPath / "vendor" / "mono" / "etc").make_preferred();

        std::string libStr = libPath.string();
        std::string etcStr = etcPath.string();

        Log::Info("Configurando rutas de Mono:");
        Log::Info("  Lib: {0}", libStr);
        Log::Info("  Etc: {0}", etcStr);

        mono_set_dirs(libStr.c_str(), etcStr.c_str());
        mono_config_parse(NULL);

        m_RootDomain = mono_jit_init("LoopieRootDomain");
        if (!m_RootDomain) {
            Log::Error("Fallo crítico: No se pudo inicializar LoopieRootDomain");
            return;
        }

        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
        mono_domain_set(m_AppDomain, true);

     
        //TODO changed hardcoded path
        rootPath = "C:/Users/paugo/Documents/GitHub/Engine/G1_LoopieEngine_Scripting";
        fs::path scriptDllPath = (rootPath / "Assets" / "Scripts" / "Loopie.Core.dll").make_preferred();


        if (fs::exists(scriptDllPath)) {
        
            MonoAssembly* coreAssembly = LoadAssembly(scriptDllPath.string());

            if (coreAssembly) {
               
                m_AssemblyImage = mono_assembly_get_image(coreAssembly);

          
                g_GameAssemblyImage = m_AssemblyImage;

               
                ScriptGlue::RegisterGlue();

                Log::Info("Scripting Core cargado exitosamente y JIT enlazado: {0}", scriptDllPath.string());
            }
            else {
                Log::Error("Se encontró el archivo pero Mono falló al cargar el Assembly.");
            }
        }
        else {
            Log::Error("No se encontró la DLL de scripts en: {0}. (¿Has compilado el proyecto C#?)", scriptDllPath.string());
        }
    }

    void ScriptingModule::OnUpdate() {
        m_WatcherTimer += 0.016f;

        if (m_WatcherTimer >= m_WatcherInterval) {
            CheckForScriptChanges();
            m_WatcherTimer = 0.0f;
        }
    }

    void ScriptingModule::CheckForScriptChanges() {

        //TODO changed hardcoded path
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