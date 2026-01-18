#pragma once
#include "Loopie/Core/Module.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <map>

namespace Loopie {
    class ScriptingModule : public Module {
    public:
        ScriptingModule() : Module("Scripting") {}
        //void OnLoad() override;
        void OnUpdate() override;
        void OnUnload() override;
        MonoAssembly* LoadAssembly(const std::string& filePath);
    private:
        MonoDomain* m_RootDomain = nullptr;
        MonoDomain* m_AppDomain = nullptr;
        MonoImage* m_AssemblyImage = nullptr;
        std::map<uint32_t, MonoObject*> m_EntityInstances;
    };
}