#include "ScriptingModule.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include <mono/metadata/mono-config.h>

namespace Loopie {
    extern void RegisterGlue();

    /*void ScriptingModule::OnLoad() {
        mono_set_dirs("vendor/mono/lib", "vendor/mono/etc");
        mono_config_parse(NULL);
        m_RootDomain = mono_jit_init("LoopieRootDomain");
        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
        mono_domain_set(m_AppDomain, true);

        RegisterGlue();

        LoadAssembly("Assets/Scripts/Scripts.dll");
    }*/

    MonoAssembly* ScriptingModule::LoadAssembly(const std::string& filePath) {
        MonoAssembly* assembly = mono_domain_assembly_open(m_AppDomain, filePath.c_str());
        if (assembly) {
            m_AssemblyImage = mono_assembly_get_image(assembly);
        }
        return assembly;
    }

    void ScriptingModule::OnUpdate() {
        if (!m_AssemblyImage) return;

        MonoClass* entityClass = mono_class_from_name(m_AssemblyImage, "Loopie", "Entity");
        MonoMethod* updateMethod = mono_class_get_method_from_name(entityClass, "OnUpdate", 0);

        for (auto const& [id, instance] : m_EntityInstances) {
            mono_runtime_invoke(updateMethod, instance, nullptr, nullptr);
        }
    }

    void ScriptingModule::OnUnload() {
        if (m_RootDomain) mono_jit_cleanup(m_RootDomain);
    }
}