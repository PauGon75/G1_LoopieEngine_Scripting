#include "ScriptGlue.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/reflection.h>

namespace Loopie {

    static void Log_Internal(MonoString* message) {
        char* msg = mono_string_to_utf8(message);
        Log::Info("[C#]: {0}", msg);
        mono_free(msg);
    }

    static void DestroyEntity_Internal(MonoString* uuidStr) {
        char* uuidCStr = mono_string_to_utf8(uuidStr);
        Scene* scene = Application::GetInstance().m_scene;
        if (scene) {
            scene->RemoveEntity(UUID(uuidCStr));
        }
        mono_free(uuidCStr);
    }

    static MonoString* CreateEntity_Internal(MonoString* name) {
        char* nameStr = mono_string_to_utf8(name);
        Scene* scene = Application::GetInstance().m_scene;
        if (!scene) return nullptr;

        auto entity = scene->CreateEntity(nameStr);
        mono_free(nameStr);

        return mono_string_new(mono_domain_get(), entity->GetUUID().Get().c_str());
    }

    void ScriptGlue::RegisterGlue() {
        mono_add_internal_call("Loopie.InternalCalls::Log", (void*)Log_Internal);
        mono_add_internal_call("Loopie.InternalCalls::CreateEntity_Internal", (void*)CreateEntity_Internal);
        mono_add_internal_call("Loopie.InternalCalls.DestroyEntity_Internal", (void*)DestroyEntity_Internal);
    }
}