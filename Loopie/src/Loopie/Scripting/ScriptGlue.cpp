#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include "Loopie/Components/Transform.h"
#include <mono/jit/jit.h>

namespace Loopie {
    static void NativeLog(MonoString* string) {
        char* cStr = mono_string_to_utf8(string);
        Log::Info("[C#]: {0}", cStr);
        mono_free(cStr);
    }

    static void Native_SetTranslation(uint32_t entityID, float x, float y, float z) {
        Scene& scene = Application::GetInstance().GetScene();
        auto entity = scene.GetEntityByUUID(entityID);
        if (entity) {
            auto& transform = entity.GetComponent<TransformComponent>();
            transform.Translation = { x, y, z };
        }
    }

    static void Native_CreateEntity(MonoString* name) {
        char* cStr = mono_string_to_utf8(name);
        Application::GetInstance().GetScene().CreateEntity(cStr);
        mono_free(cStr);
    }

    void RegisterGlue() {
        mono_add_internal_call("Loopie.InternalCalls::NativeLog", (void*)NativeLog);
        mono_add_internal_call("Loopie.InternalCalls::Native_SetTranslation", (void*)Native_SetTranslation);
        mono_add_internal_call("Loopie.InternalCalls::Native_CreateEntity", (void*)Native_CreateEntity);
    }
}