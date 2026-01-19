#pragma once

#include "Loopie/Core/Module.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace Loopie {

    class ScriptingModule : public Module {
    public:
        ScriptingModule() : Module("Scripting") {}

        void OnLoad() override;
        void OnUpdate() override;
        void OnUnload() override;

        void CheckForScriptChanges();
        MonoAssembly* LoadAssembly(const std::string& filePath);
        std::unordered_map<std::string, std::filesystem::file_time_type> m_FileWatchMap;
    private:
        MonoDomain* m_RootDomain = nullptr;
        MonoDomain* m_AppDomain = nullptr;
        MonoImage* m_AssemblyImage = nullptr;

        float m_WatcherTimer = 0.0f;
        const float m_WatcherInterval = 2.0f;
    };

}