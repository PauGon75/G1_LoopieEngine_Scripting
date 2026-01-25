#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/reflection.h>
#include "Loopie/Scripting/ScriptingModule.h"
#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>

#include <imgui.h>       
#include <glm/glm.hpp>   
#include <glm/gtc/type_ptr.hpp>
extern MonoImage* g_GameAssemblyImage;

namespace Loopie {

    static void InjectEntityID(MonoObject* instance, const std::string& uuid) {
        if (!instance) return;

        MonoClass* monoClass = mono_object_get_class(instance);
        MonoClassField* entityIDField = mono_class_get_field_from_name(monoClass, "EntityID");

        if (!entityIDField) {
            MonoClass* parentClass = mono_class_get_parent(monoClass);
            if (parentClass) entityIDField = mono_class_get_field_from_name(parentClass, "EntityID");
        }

        if (entityIDField) {
            MonoString* monoStr = mono_string_new(ScriptingModule::GetAppDomain(), uuid.c_str());
            mono_field_set_value(instance, entityIDField, monoStr);
        }
        else {
            Log::Error("[ScriptComponent] CRITICO: No se encuentra 'public string EntityID' en el script C#.");
        }
    }

    ScriptComponent::~ScriptComponent() {
        m_instance = nullptr;
    }
    void ScriptComponent::EnsureInitialized()
    {
        new (&m_scriptName) std::string();

        m_instance = nullptr;
    }
    void ScriptComponent::Init() {
        if (!m_scriptName.empty() && !m_instance) {
            SetScript(m_scriptName);
        }

        if (m_instance) {
            auto owner = m_owner.lock();
            if (owner) {
                InjectEntityID(m_instance, owner->GetUUID().Get());
            }
        }
    }

    void ScriptComponent::SetScript(const std::string& name) {
        m_isBound = false;
        m_scriptName = name;
        m_instance = nullptr;

        if (!g_GameAssemblyImage || name.empty()) return;

        const char* namespacesToTry[] = { "", "Scripts", "Loopie", "Loopie.Scripts" };
        MonoClass* monoClass = nullptr;

        for (const char* ns : namespacesToTry) {
            monoClass = mono_class_from_name(g_GameAssemblyImage, ns, name.c_str());
            if (monoClass) break;
        }

        if (!monoClass) {
            Log::Error("Scripting: No se encontro la clase '{0}'", name);
            return;
        }

        MonoDomain* appDomain = ScriptingModule::GetAppDomain();
        m_instance = mono_object_new(appDomain, monoClass);

        if (m_instance) {
            mono_runtime_object_init(m_instance);

            auto owner = m_owner.lock();
            if (owner) {
                InjectEntityID(m_instance, owner->GetUUID().Get());
            }

            m_startMethod = mono_class_get_method_from_name(monoClass, "Start", 0);
            m_updateMethod = mono_class_get_method_from_name(monoClass, "Update", 1);
            m_isBound = true;
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
        JsonNode node = parent.CreateObjectField("ScriptComponent");
        node.CreateField("ScriptName", m_scriptName);
        return node;
    }

    void ScriptComponent::Deserialize(const JsonNode& data) {
        if (data.Contains("ScriptName")) {
            auto result = data.GetValue<std::string>("ScriptName");
            if (result.Found) SetScript(result.Result);
        }
    }


    void ScriptComponent::OnImGuiRender()
    {
        if (!m_instance) return;

        MonoClass* klass = mono_object_get_class(m_instance);
        void* iter = nullptr;
        MonoClassField* field;

        while ((field = mono_class_get_fields(klass, &iter)))
        {
            uint32_t flags = mono_field_get_flags(field);
            if ((flags & MONO_FIELD_ATTR_PUBLIC) == 0) continue;

            const char* fieldName = mono_field_get_name(field);
            MonoType* type = mono_field_get_type(field);
            int typeEnum = mono_type_get_type(type);

            if (typeEnum == MONO_TYPE_R4)
            {
                float value = 0.0f;
                mono_field_get_value(m_instance, field, &value);
                if (ImGui::DragFloat(fieldName, &value, 0.1f))
                    mono_field_set_value(m_instance, field, &value);
            }
            else if (typeEnum == MONO_TYPE_I4)
            {
                int value = 0;
                mono_field_get_value(m_instance, field, &value);
                if (ImGui::DragInt(fieldName, &value))
                    mono_field_set_value(m_instance, field, &value);
            }
            else if (typeEnum == MONO_TYPE_BOOLEAN)
            {
                bool value = false;
                mono_field_get_value(m_instance, field, &value);
                if (ImGui::Checkbox(fieldName, &value))
                    mono_field_set_value(m_instance, field, &value);
            }
            else if (typeEnum == MONO_TYPE_VALUETYPE)
            {
                char* typeName = mono_type_get_name(type);
                if (std::string(typeName).find("Vector3") != std::string::npos)
                {
                    glm::vec3 value;
                    mono_field_get_value(m_instance, field, &value);
                    if (ImGui::DragFloat3(fieldName, glm::value_ptr(value), 0.1f))
                        mono_field_set_value(m_instance, field, &value);
                }
                mono_free(typeName);
            }
        }
    }
}