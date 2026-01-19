#pragma once

#include "Loopie/Components/Component.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Loopie {

    class ScriptComponent : public Component {
    public:
        // Constructor por defecto para que AddComponent<T> funcione
        ScriptComponent() = default;
        explicit ScriptComponent(Entity* owner);
        ~ScriptComponent() override;

        // Implementacion obligatoria de las funciones de Component.h
        void Init() override;
        void Update() override;
        JsonNode Serialize(JsonNode& parent) const override;
        void Deserialize(const JsonNode& data) override;

        void SetScript(const std::string& name);
        std::string GetScriptName() const { return m_scriptName; }

    private:
        std::string m_scriptName;
        MonoObject* m_instance = nullptr;
        MonoMethod* m_startMethod = nullptr;
        MonoMethod* m_updateMethod = nullptr;
        bool m_startCalled = false;
    };
}