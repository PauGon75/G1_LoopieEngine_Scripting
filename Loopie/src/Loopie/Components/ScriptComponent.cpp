#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Scene/Entity.h" 
#include <mono/metadata/reflection.h>
#include <mono/metadata/attrdefs.h>

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

      
        if (m_instance) {
            JsonNode fieldsNode = node.CreateObjectField("Fields");
            MonoClass* klass = mono_object_get_class(m_instance);
            void* iter = nullptr;
            MonoClassField* field;

            while ((field = mono_class_get_fields(klass, &iter))) {
               
                uint32_t flags = mono_field_get_flags(field);
                if ((flags & MONO_FIELD_ATTR_PUBLIC) == 0) continue;

                const char* fieldName = mono_field_get_name(field);
                MonoType* type = mono_field_get_type(field);
                int monoType = mono_type_get_type(type);

                
                switch (monoType) {
                case MONO_TYPE_I4: { // int
                    int val;
                    mono_field_get_value(m_instance, field, &val);
                    fieldsNode.CreateField(fieldName, val);
                    break;
                }
                case MONO_TYPE_R4: {
                    float val;
                    mono_field_get_value(m_instance, field, &val);
                    fieldsNode.CreateField(fieldName, val);
                    break;
                }
                case MONO_TYPE_BOOLEAN: { 
                    bool val;
                    mono_field_get_value(m_instance, field, &val);
                    fieldsNode.CreateField(fieldName, val);
                    break;
                }
                case MONO_TYPE_STRING: { 
                    MonoString* strObj;
                    mono_field_get_value(m_instance, field, &strObj);
                    if (strObj) {
                        char* cStr = mono_string_to_utf8(strObj);
                        fieldsNode.CreateField(fieldName, std::string(cStr));
                        mono_free(cStr);
                    }
                    else {
                        fieldsNode.CreateField(fieldName, std::string(""));
                    }
                    break;
                }
                                    
                }
            }
        }

        return node;
    }

    void ScriptComponent::Deserialize(const JsonNode& data) {
        
        if (data.Contains("ScriptName")) {
            auto result = data.GetValue<std::string>("ScriptName");
            if (result.Found && !result.Result.empty()) {
                
                SetScript(result.Result);
            }
        }

       
        if (!m_instance) return;

        
        if (data.Contains("Fields")) {
            JsonNode fieldsNode = data.Child("Fields");

           
            std::vector<std::string> keys = fieldsNode.GetObjectKeys();
            MonoClass* klass = mono_object_get_class(m_instance);

            for (const std::string& fieldName : keys) {
              
                MonoClassField* field = mono_class_get_field_from_name(klass, fieldName.c_str());
                if (!field) continue;

                MonoType* type = mono_field_get_type(field);
                int monoType = mono_type_get_type(type);

              
                switch (monoType) {
                case MONO_TYPE_I4: {
                    auto res = fieldsNode.GetValue<int>(fieldName);
                    if (res.Found) mono_field_set_value(m_instance, field, &res.Result);
                    break;
                }
                case MONO_TYPE_R4: {
                    auto res = fieldsNode.GetValue<float>(fieldName);
                    if (res.Found) mono_field_set_value(m_instance, field, &res.Result);
                    break;
                }
                case MONO_TYPE_BOOLEAN: {
                    auto res = fieldsNode.GetValue<bool>(fieldName);
                    if (res.Found) mono_field_set_value(m_instance, field, &res.Result);
                    break;
                }
                case MONO_TYPE_STRING: {
                    auto res = fieldsNode.GetValue<std::string>(fieldName);
                    if (res.Found) {
                        MonoString* str = mono_string_new(mono_domain_get(), res.Result.c_str());
                        mono_field_set_value(m_instance, field, str);
                    }
                    break;
                }
                }
            }
        }
    }
}

