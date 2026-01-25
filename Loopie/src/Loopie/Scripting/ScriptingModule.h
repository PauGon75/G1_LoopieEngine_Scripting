#pragma once

#include "Loopie/Core/Module.h"
#include "Loopie/Core/Application.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace Loopie {

    class ScriptingModule : public Module {
    public:
        ScriptingModule() : Module("Scripting") { s_Instance = this; }

        void OnLoad() override;
        void OnUpdate() override;
        void OnUnload() override;

        static void CheckForScriptChanges();

        // --- NUEVO: Función para sincronizar Assets -> Core ---
        static void SyncScriptsFromAssetsToCore();
        // ------------------------------------------------------

        static void ReloadAssembly();
        static void LoadCoreAssembly();
        void InitializeScriptInstance(MonoObject* instance, const std::string& uuid);

        static MonoAssembly* LoadAssembly(const std::string& filePath);
        static MonoDomain* GetAppDomain() { return m_AppDomain; }
        static MonoAssembly* GetAssembly() { return m_CoreAssembly; }
        static MonoImage* GetAssemblyImage() { return m_AssemblyImage; }

        static MonoDomain* m_RootDomain;
        static MonoDomain* m_AppDomain;
        static MonoImage* m_AssemblyImage;
        static MonoAssembly* m_CoreAssembly;

        static std::unordered_map<std::string, std::filesystem::file_time_type> m_FileWatchMap;
        float m_WatcherTimer = 0.0f;
        const float m_WatcherInterval = 1.0f; // He bajado esto a 1s para que sea más rápido

        static std::vector<std::string> GetAvailableScripts();

        static std::string GetScriptsPath() {
            // Ajusta esto si tu ruta real es distinta
            return "../../../Assets/Scripts";
        }

        static void CreateScriptAsset(const std::filesystem::path& directory, const std::string& name);
    private:
        static ScriptingModule* s_Instance;
    };
}