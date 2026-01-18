#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Scene/Entity.h" 

extern MonoImage* g_GameAssemblyImage;

namespace Loopie {

    ScriptComponent::ScriptComponent(Entity* owner)
    {
   
    }

    ScriptComponent::~ScriptComponent() {
        m_instance = nullptr;
    }

    void ScriptComponent::Init() {
        if (!m_scriptName.empty() && !m_instance) {
            SetScript(m_scriptName);
        }
    }

    void ScriptComponent::SetScript(const std::string& name) {
        if (!g_GameAssemblyImage) return;

        m_scriptName = name;
        MonoClass* monoClass = mono_class_from_name(g_GameAssemblyImage, "", name.c_str());

        if (!monoClass) {
           
            return;
        }

        m_instance = mono_object_new(mono_domain_get(), monoClass);
        if (!m_instance) return;

        mono_runtime_object_init(m_instance);

        m_startMethod = mono_class_get_method_from_name(monoClass, "Start", 0);
        m_updateMethod = mono_class_get_method_from_name(monoClass, "Update", 1);
    }

    void ScriptComponent::Update() {
        if (!m_instance || !m_updateMethod) return;

        if (!m_startCalled && m_startMethod) {
            MonoObject* exc = nullptr;
            mono_runtime_invoke(m_startMethod, m_instance, nullptr, &exc);
            if (exc) mono_print_unhandled_exception(exc);
            m_startCalled = true;
        }


        float dt = 0.016f;
        void* args[1];
        args[0] = &dt;

        MonoObject* exception = nullptr;
        mono_runtime_invoke(m_updateMethod, m_instance, args, &exception);

        if (exception) mono_print_unhandled_exception(exception);
    }

    JsonNode ScriptComponent::Serialize(JsonNode& parent) const {
      
        JsonNode node = parent.CreateObjectField("ScriptComponent");


        node.CreateField("ScriptName", m_scriptName);

        return node;
    }

    void ScriptComponent::Deserialize(const JsonNode& data) {
      
        if (data.Contains("ScriptName")) {

          
            auto result = data.GetValue<std::string>("ScriptName");

            if (result.Found) {
                m_scriptName = result.Result;
            }
        }
    }
}