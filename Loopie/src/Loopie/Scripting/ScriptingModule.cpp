#include "ScriptingModule.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Resources/ScriptResource.h"
#include "Loopie/Importers/ScriptImporter.h"
#include <mono/metadata/mono-config.h>
#include "Loopie/Resources/ResourceManager.h"

namespace Loopie {

    extern void RegisterGlue();

    void ScriptingModule::OnLoad() {
        mono_set_dirs("vendor/mono/lib", "vendor/mono/etc");
        mono_config_parse(NULL);

        m_RootDomain = mono_jit_init("LoopieRootDomain");
        m_AppDomain = mono_domain_create_appdomain((char*)"LoopieAppDomain", NULL);
        mono_domain_set(m_AppDomain, true);

        RegisterGlue();

        LoadAssembly("Assets/Scripts/Scripts.dll");
    }

    void ScriptingModule::OnUpdate() {
        m_WatcherTimer += 0.016f;
        if (m_WatcherTimer >= m_WatcherInterval) {
            CheckForScriptChanges();
            m_WatcherTimer = 0.0f;
        }

        if (!m_AssemblyImage) return;

        for (auto& instance : m_ActiveInstances) {
            MonoClass* monoClass = mono_object_get_class(instance);
            MonoMethod* updateMethod = mono_class_get_method_from_name(monoClass, "Update", 1);

            if (updateMethod) {
                float dt = 0.016f;
                void* args[1];
                args[0] = &dt;
                mono_runtime_invoke(updateMethod, instance, args, nullptr);
            }
        }
    }

    void ScriptingModule::CheckForScriptChanges() {
        std::string scriptsPath = "Assets/Scripts"; 
        if (!std::filesystem::exists(scriptsPath)) return;

        for (auto& entry : std::filesystem::recursive_directory_iterator(scriptsPath)) {
            if (entry.path().extension() == ".cs") {
                std::string pathStr = entry.path().string();
                auto lastWrite = std::filesystem::last_write_time(entry.path());

                auto resource = ResourceManager::GetResourceByPath<ScriptResource>(pathStr);

                if (!resource) {
                    ScriptImporter::Import(pathStr);
                }
                else if (resource->NeedsRecompile(lastWrite)) {
                    Log::Info("ScriptingModule: Cambio detectado en {0}. Recompilando...", entry.path().filename().string());
                    ScriptImporter::Compile(resource);
                    resource->SetLastWriteTime(lastWrite);
                }
            }
        }
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