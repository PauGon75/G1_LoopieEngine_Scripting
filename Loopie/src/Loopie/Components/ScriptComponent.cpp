#include "ScriptComponent.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/reflection.h>
#include "Loopie/Scripting/ScriptingModule.h"

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

        if (!g_GameAssemblyImage || name.empty()) {
            Log::Debug("Scripting: Assembly image no disponible o nombre vacio.");
            return;
        }

        const char* namespacesToTry[] = {
            "",
            "Scripts",
            "Loopie",
            "Loopie.Scripts"
        };

        MonoClass* monoClass = nullptr;
        std::string foundNamespace = "";

        for (const char* ns : namespacesToTry) {
            monoClass = mono_class_from_name(g_GameAssemblyImage, ns, name.c_str());
            if (monoClass) {
                foundNamespace = std::string(ns);
                Log::Debug("Scripting: Clase '{0}' encontrada en namespace '{1}'",
                    name, foundNamespace.empty() ? "(global)" : foundNamespace);
                break;
            }
        }

        if (!monoClass) {
            Log::Error("Scripting: No se encontro la clase '{0}'", name);
            return;
        }

        const char* className = mono_class_get_name(monoClass);
        if (std::string(className) == "<Module>") {
            return;
        }

        // DIAGNÓSTICO EXTENSO
        Log::Debug("=== DIAGNOSTICO DE CLASE ===");
        Log::Debug("Nombre: {0}", mono_class_get_name(monoClass));
        Log::Debug("Namespace: {0}", mono_class_get_namespace(monoClass));

        // Verificar flags de la clase
        uint32_t flags = mono_class_get_flags(monoClass);
        Log::Debug("Flags: 0x{0:X}", flags);

        // Verificar si es una clase válida
        MonoType* classType = mono_class_get_type(monoClass);
        if (classType) {
            Log::Debug("Type: {0}", mono_type_get_name(classType));
        }

        // Verificar tamaño de la clase
        int32_t classSize = mono_class_instance_size(monoClass);
        Log::Debug("Instance size: {0} bytes", classSize);

        // Verificar si tiene campos
        void* iter = nullptr;
        MonoClassField* field;
        int fieldCount = 0;
        while ((field = mono_class_get_fields(monoClass, &iter))) {
            fieldCount++;
        }
        Log::Debug("Field count: {0}", fieldCount);

        // Verificar constructores
        iter = nullptr;
        MonoMethod* method;
        bool hasDefaultConstructor = false;
        int methodCount = 0;
        while ((method = mono_class_get_methods(monoClass, &iter))) {
            methodCount++;
            const char* methodName = mono_method_get_name(method);
            if (strcmp(methodName, ".ctor") == 0) {
                MonoMethodSignature* sig = mono_method_signature(method);
                int paramCount = mono_signature_get_param_count(sig);
                Log::Debug("Constructor encontrado con {0} parametros", paramCount);
                if (paramCount == 0) {
                    hasDefaultConstructor = true;
                }
            }
        }
        Log::Debug("Method count: {0}", methodCount);
        Log::Debug("Has default constructor: {0}", hasDefaultConstructor ? "SI" : "NO");

        // Verificar el parent class
        MonoClass* parentClass = mono_class_get_parent(monoClass);
        if (parentClass) {
            Log::Debug("Parent class: {0}", mono_class_get_name(parentClass));
        }

        Log::Debug("=== FIN DIAGNOSTICO ===");

        MonoDomain* appDomain = ScriptingModule::GetAppDomain();

        Log::Debug("Intentando mono_object_new...");
        m_instance = mono_object_new(appDomain, monoClass);

        if (!m_instance) {
            Log::Error("mono_object_new fallo - Posibles causas:");
            Log::Error("  1. La clase require constructor con parametros");
            Log::Error("  2. DLL compilado con version incompatible de .NET");
            Log::Error("  3. Problema con el dominio de Mono");

            // Último intento: llamar al constructor manualmente
            Log::Debug("Intentando crear objeto sin constructor...");
            m_instance = mono_object_new(appDomain, monoClass);

            if (!m_instance) {
                Log::Error("Fallo total. Revisa la compilacion del DLL.");
                return;
            }
        }

        Log::Info("¡Instancia creada!");
        mono_runtime_object_init(m_instance);

        m_startMethod = mono_class_get_method_from_name(monoClass, "Start", 0);
        m_updateMethod = mono_class_get_method_from_name(monoClass, "Update", 1);

        m_isBound = true;
        Log::Info("'{0}' vinculado exitosamente!", name);
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