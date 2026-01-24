#pragma once

#include "Loopie/Components/Component.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <Loopie/Scene/Entity.h>

namespace Loopie {
    class Entity;

    class ScriptComponent : public Component {
    public:
        DEFINE_TYPE(ScriptComponent)

            ScriptComponent() = default;
        ~ScriptComponent() override;

        void Init() override;
        void Update() override;
        JsonNode Serialize(JsonNode& parent) const override;
        void Deserialize(const JsonNode& data) override;

        void SetScript(const std::string& name);
        const std::string& GetScriptName() const { return m_scriptName; }
        bool IsBound() const { return m_isBound; }

    private:
        std::string m_scriptName;
        MonoClass* m_scriptClass = nullptr;
        MonoObject* m_instance = nullptr;
        MonoMethod* m_startMethod = nullptr;
        MonoMethod* m_updateMethod = nullptr;

        bool m_startCalled = false;
        bool m_isBound = false;
    };
}