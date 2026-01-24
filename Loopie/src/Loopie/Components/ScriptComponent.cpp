#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/reflection.h>
#include "Loopie/Scripting/ScriptingModule.h"

extern MonoImage* g_GameAssemblyImage;

namespace Loopie {

    // HELPER: Inyecta el ID sin repetir código
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
            // Log::Debug("Inyectado ID: {0}", uuid); // Descomenta si quieres spam
        }
        else {
            Log::Error("[ScriptComponent] CRITICO: No se encuentra 'public string EntityID' en el script C#.");
        }
    }

    ScriptComponent::~ScriptComponent() {
        m_instance = nullptr;
    }

    void ScriptComponent::Init() {
        // 1. Aseguramos que el script esté cargado
        if (!m_scriptName.empty() && !m_instance) {
            SetScript(m_scriptName);
        }

        // 2. FORZAMOS la inyección siempre en el Init (por si Deserialize lo cargó sin dueño)
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

            // 3. INYECCIÓN INMEDIATA (Para que funcione al Recargar Assembly o Hot-Reload)
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

        float dt = 0.016f; // TODO: Usar Time.deltaTime real
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
}