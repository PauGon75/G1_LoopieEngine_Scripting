#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/reflection.h>

extern MonoImage* g_GameAssemblyImage;

namespace Loopie {

    // Arregla el error de herencia pasando el owner a la base
    ScriptComponent::ScriptComponent(Entity* owner) : Component()
    {
        // Nota: Si tu clase base Component requiere owner en el constructor, usalo aqui
        m_owner = owner;
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
        m_isBound = false;
        m_scriptName = name;

        if (!g_GameAssemblyImage) {
            Log::Error("Scripting: g_GameAssemblyImage es NULL. �Cargaste la DLL?");
            return;
        }

        // Intento 1: Namespace vac�o (como lo tienes ahora)
        m_scriptClass = mono_class_from_name(g_GameAssemblyImage, "", name.c_str());

        // Intento 2: Namespace "Loopie" (por si acaso)
        if (!m_scriptClass) {
            m_scriptClass = mono_class_from_name(g_GameAssemblyImage, "Loopie", name.c_str());
        }

        if (!m_scriptClass) {
            Log::Error("Scripting: No se encontro la clase '{0}' en la DLL.", name);
            return;
        }

        m_instance = mono_object_new(mono_domain_get(), m_scriptClass);
        if (m_instance) {
            mono_runtime_object_init(m_instance);
            m_isBound = true; // ESTO ES LO QUE LO PONE VERDE
            Log::Debug("Scripting: Clase '{0}' vinculada y marcada como Bound.", name);
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

    void ScriptComponent::SetEntityReference(Entity* cppEntity) {
        
        MonoImage* image = mono_assembly_get_image(ScriptingModule::GetAssembly());
        MonoClass* entityClass = mono_class_from_name(image, "Loopie", "Entity");

        if (!entityClass)
        {
            Log::Error("Entity class in C# not found");
            return;
        }

        MonoObject* entityInstance = mono_object_new(ScriptingModule::GetAppDomain(), entityClass);

        MonoMethod* constructor = mono_class_get_method_from_name(entityClass, ".ctor", 1);

        if (constructor)
        {
            void* args[1];
            UUID entityID = cppEntity->GetUUID();
            args[0] = &entityID;

            mono_runtime_invoke(constructor, entityInstance, args, nullptr);
        }
        else
        {
            mono_runtime_object_init(entityInstance);

            MonoClassField* idField = mono_class_get_field_from_name(entityClass, "entityID");
            if (idField)
            {
                UUID entityID = cppEntity->GetUUID();
                mono_field_set_value(entityInstance, idField, &entityID);
            }
        }

        MonoClassField* entityField = mono_class_get_field_from_name(m_scriptClass, "entity");

        if (entityField)
        {
            mono_field_set_value(m_instance, entityField, entityInstance);
        }
        else
        {
            Log::Error("No se encontr� el campo 'entity' en LoopieScript");
        }
    }
}