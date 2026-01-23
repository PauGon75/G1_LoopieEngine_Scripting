#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/reflection.h>

extern MonoImage* g_GameAssemblyImage;

namespace Loopie {

    ScriptComponent::~ScriptComponent() {
        m_instance = nullptr;
    }

    void ScriptComponent::Init() {
        if (!m_scriptName.empty() && !m_instance) {
            SetScript(m_scriptName);
        }
    }

    void ScriptComponent::SetScript(const std::string& name) {
        m_isBound = false;
        m_scriptName = name;
        m_instance = nullptr;

        if (!g_GameAssemblyImage || name.empty()) return;

        MonoClass* monoClass = mono_class_from_name(g_GameAssemblyImage, "Scripts", name.c_str());

        if (!monoClass) {
            monoClass = mono_class_from_name(g_GameAssemblyImage, "", name.c_str());
        }

        if (!monoClass) {
            Log::Debug("Scripting: No se encontro la clase '{0}' en 'Scripts' ni en el global.", name); 
            return;
        }

        if (std::string(mono_class_get_name(monoClass)) == "<Module>") return;

        m_instance = mono_object_new(mono_domain_get(), monoClass);
        if (m_instance) {
            mono_runtime_object_init(m_instance);

            m_startMethod = mono_class_get_method_from_name(monoClass, "Start", 0);
            m_updateMethod = mono_class_get_method_from_name(monoClass, "Update", 1);

            m_isBound = true;
            Log::Debug("Scripting: Clase '{0}' vinculada correctamente.", name); 
        }
    }

    void ScriptComponent::Update() {
        if (!m_instance || !m_updateMethod) return;

        if (!m_startCalled && m_startMethod) {
            mono_runtime_invoke(m_startMethod, m_instance, nullptr, nullptr);
            m_startCalled = true;
        }

        float dt = 0.016f;
        void* args[1] = { &dt };
        mono_runtime_invoke(m_updateMethod, m_instance, args, nullptr);
    }

    JsonNode ScriptComponent::Serialize(JsonNode& parent) const {
        // Creamos el objeto exactamente como en MeshRenderer
        JsonNode node = parent.CreateObjectField("ScriptComponent");
        node.CreateField("ScriptName", m_scriptName);
        return node;
    }

    void ScriptComponent::Deserialize(const JsonNode& data) {
        if (data.Contains("ScriptName")) {
            auto result = data.GetValue<std::string>("ScriptName");
            if (result.Found) {
                SetScript(result.Result);
            }
        }
    }
}