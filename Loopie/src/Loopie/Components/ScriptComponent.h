#pragma once

#include "Loopie/Components/Component.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Loopie {

    class ScriptComponent : public Component {
    public:
     
        explicit ScriptComponent(Entity* owner);
        ~ScriptComponent();

      
        void Init() override;
        void Update() override;
      

     
        void SetScript(const std::string& name);
        std::string GetScriptName() const { return m_scriptName; }

      
        JsonNode Serialize(JsonNode& parent) const override;
        void Deserialize(const JsonNode& data) override;

    private:
        std::string m_scriptName;

       
        MonoObject* m_instance = nullptr;
        MonoMethod* m_startMethod = nullptr;
        MonoMethod* m_updateMethod = nullptr;

        bool m_startCalled = false;
    };
}