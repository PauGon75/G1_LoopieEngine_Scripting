#pragma once

#include "Loopie/Components/Component.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Loopie {

    class ScriptComponent : public Component {
    public:
        // Usamos tu macro para que GetTypeID devuelva size_t correctamente
        DEFINE_TYPE(ScriptComponent)

            ScriptComponent() = default;
        ~ScriptComponent() override;

        // Implementación obligatoria de Component.h
        void Init() override;
        void Update() override;
        JsonNode Serialize(JsonNode& parent) const override;
        void Deserialize(const JsonNode& data) override;

        void SetScript(const std::string& name);
        const std::string& GetScriptName() const { return m_scriptName; }
        bool IsBound() const { return m_isBound; } // Para que el Inspector lo sepa
    private:
        std::string m_scriptName;
        MonoObject* m_instance = nullptr;
        MonoMethod* m_startMethod = nullptr;
        MonoMethod* m_updateMethod = nullptr;
        bool m_startCalled = false;
        bool m_isBound = false; // <--- Nuevo flag
    };
}   