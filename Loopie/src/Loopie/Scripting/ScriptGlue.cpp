#include "ScriptGlue.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Camera.h"
#include <mono/metadata/object.h>

namespace Loopie {

    static std::shared_ptr<Entity> GetEntityByUUID(MonoString* uuidStr) {
        if (!uuidStr) return nullptr;
        Scene* scene = Application::GetInstance().m_scene;
        if (!scene) return nullptr;

        char* cStr = mono_string_to_utf8(uuidStr);
        if (!cStr) return nullptr;

        UUID uuid(cStr);
        mono_free(cStr);

        return scene->GetEntity(uuid);
    }

    static void Log_Native(MonoString* message) {
        if (!message) return;
        char* cStr = mono_string_to_utf8(message);
        Log::Info("[C#] {0}", cStr);
        mono_free(cStr);
    }

    static void LogWarning_Native(MonoString* message) {
        if (!message) return;
        char* cStr = mono_string_to_utf8(message);
        Log::Warn("[C#] {0}", cStr);
        mono_free(cStr);
    }

    static void LogError_Native(MonoString* message) {
        if (!message) return;
        char* cStr = mono_string_to_utf8(message);
        Log::Error("[C#] {0}", cStr);
        mono_free(cStr);
    }

    static MonoString* Entity_Create_Native(MonoString* name) {
        std::string entityName = "New Entity";
        if (name) {
            char* cStr = mono_string_to_utf8(name);
            if (cStr) {
                entityName = cStr;
                mono_free(cStr);
            }
        }

        Scene* scene = Application::GetInstance().m_scene;
        if (!scene) return nullptr;

        std::shared_ptr<Entity> entity = scene->CreateEntity(entityName);
        return mono_string_new(mono_domain_get(), entity->GetUUID().Get().c_str());
    }

    static void Entity_Destroy_Native(MonoString* uuidStr) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) Application::GetInstance().m_scene->RemoveEntity(entity->GetUUID());
    }

    static MonoString* Entity_GetName_Native(MonoString* uuidStr) {
        auto entity = GetEntityByUUID(uuidStr);
        return entity ? mono_string_new(mono_domain_get(), entity->GetName().c_str()) : mono_string_new(mono_domain_get(), "");
    }

    static void Entity_SetName_Native(MonoString* uuidStr, MonoString* name) {
        if (!name) return;
        char* cStr = mono_string_to_utf8(name);
        std::string newName(cStr);
        mono_free(cStr);
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) entity->SetName(newName);
    }

    static bool Entity_GetActive_Native(MonoString* uuidStr) {
        auto entity = GetEntityByUUID(uuidStr);
        return entity ? entity->GetIsActive() : false;
    }

    static void Entity_SetActive_Native(MonoString* uuidStr, bool active) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) entity->SetIsActive(active);
    }

    static bool Entity_HasComponent_Native(MonoString* uuidStr, MonoString* componentType) {
        if (!componentType) return false;
        char* cStr = mono_string_to_utf8(componentType);
        std::string type(cStr);
        mono_free(cStr);

        auto entity = GetEntityByUUID(uuidStr);
        if (!entity) return false;

        if (type == "Transform") return entity->HasComponent<Transform>();
        if (type == "MeshRenderer") return entity->HasComponent<MeshRenderer>();
        if (type == "Camera") return entity->HasComponent<Camera>();
        return false;
    }

    static bool Entity_AddComponent_Native(MonoString* uuidStr, MonoString* componentType) {
        if (!componentType) return false;
        char* cStr = mono_string_to_utf8(componentType);
        std::string type(cStr);
        mono_free(cStr);

        auto entity = GetEntityByUUID(uuidStr);
        if (!entity) return false;

        if (type == "MeshRenderer" && !entity->HasComponent<MeshRenderer>()) {
            entity->AddComponent<MeshRenderer>();
            return true;
        }
        if (type == "Camera" && !entity->HasComponent<Camera>()) {
            entity->AddComponent<Camera>();
            return true;
        }
        return false;
    }

    static void Entity_RemoveComponent_Native(MonoString* uuidStr, MonoString* componentType) {
        if (!componentType) return;
        char* cStr = mono_string_to_utf8(componentType);
        std::string type(cStr);
        mono_free(cStr);

        auto entity = GetEntityByUUID(uuidStr);
        if (entity && type == "MeshRenderer") entity->RemoveComponent<MeshRenderer>();
    }

    static void Transform_GetPosition_Native(MonoString* uuidStr, float* x, float* y, float* z) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) {
            glm::vec3 pos = entity->GetTransform()->GetLocalPosition();
            *x = pos.x; *y = pos.y; *z = pos.z;
        }
    }

    static void Transform_SetPosition_Native(MonoString* uuidStr, float x, float y, float z) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) entity->GetTransform()->SetLocalPosition({ x, y, z });
    }

    static void Transform_GetRotation_Native(MonoString* uuidStr, float* x, float* y, float* z, float* w) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) {
            glm::quat rot = entity->GetTransform()->GetLocalRotation();
            *x = rot.x; *y = rot.y; *z = rot.z; *w = rot.w;
        }
    }

    static void Transform_SetRotation_Native(MonoString* uuidStr, float x, float y, float z, float w) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) entity->GetTransform()->SetLocalRotation(glm::quat(w, x, y, z));
    }

    static void Transform_GetScale_Native(MonoString* uuidStr, float* x, float* y, float* z) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) {
            glm::vec3 scale = entity->GetTransform()->GetLocalScale();
            *x = scale.x; *y = scale.y; *z = scale.z;
        }
    }

    static void Transform_SetScale_Native(MonoString* uuidStr, float x, float y, float z) {
        auto entity = GetEntityByUUID(uuidStr);
        if (entity) entity->GetTransform()->SetLocalScale({ x, y, z });
    }

    static bool Input_IsKeyPressed_Native(int keyCode) {
        return Application::GetInstance().GetInputEvent().GetKeyStatus((SDL_Scancode)keyCode) == KeyState::DOWN;
    }

    static bool Input_IsKeyDown_Native(int keyCode) {
        return Application::GetInstance().GetInputEvent().GetKeyStatus((SDL_Scancode)keyCode) == KeyState::DOWN;
    }

    static bool Input_IsKeyReleased_Native(int keyCode) {
        return Application::GetInstance().GetInputEvent().GetKeyStatus((SDL_Scancode)keyCode) == KeyState::UP;
    }

    static void Input_GetMousePosition_Native(float* x, float* y) {
        auto pos = Application::GetInstance().GetInputEvent().GetMousePosition();
        *x = pos.x; *y = pos.y;
    }

    static void Input_GetMouseDelta_Native(float* x, float* y) {
        auto delta = Application::GetInstance().GetInputEvent().GetMouseDelta();
        *x = (float)delta.x; *y = (float)delta.y;
    }

    static bool Input_IsMouseButtonPressed_Native(int button) {
        //return Application::GetInstance().GetInputEvent().IsMouseButtonPressed(button);
        return true;
    }

    void ScriptGlue::RegisterGlue() {
        mono_add_internal_call("Loopie.InternalCalls::Log", (void*)Log_Native);
        mono_add_internal_call("Loopie.InternalCalls::LogWarning", (void*)LogWarning_Native);
        mono_add_internal_call("Loopie.InternalCalls::LogError", (void*)LogError_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_Create", (void*)Entity_Create_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_Destroy", (void*)Entity_Destroy_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_GetName", (void*)Entity_GetName_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_SetName", (void*)Entity_SetName_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_GetActive", (void*)Entity_GetActive_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_SetActive", (void*)Entity_SetActive_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_HasComponent", (void*)Entity_HasComponent_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_AddComponent", (void*)Entity_AddComponent_Native);
        mono_add_internal_call("Loopie.InternalCalls::Entity_RemoveComponent", (void*)Entity_RemoveComponent_Native);
        mono_add_internal_call("Loopie.InternalCalls::Transform_GetPosition", (void*)Transform_GetPosition_Native);
        mono_add_internal_call("Loopie.InternalCalls::Transform_SetPosition", (void*)Transform_SetPosition_Native);
        mono_add_internal_call("Loopie.InternalCalls::Transform_GetRotation", (void*)Transform_GetRotation_Native);
        mono_add_internal_call("Loopie.InternalCalls::Transform_SetRotation", (void*)Transform_SetRotation_Native);
        mono_add_internal_call("Loopie.InternalCalls::Transform_GetScale", (void*)Transform_GetScale_Native);
        mono_add_internal_call("Loopie.InternalCalls::Transform_SetScale", (void*)Transform_SetScale_Native);
        mono_add_internal_call("Loopie.InternalCalls::Input_IsKeyPressed", (void*)Input_IsKeyPressed_Native);
        mono_add_internal_call("Loopie.InternalCalls::Input_IsKeyDown", (void*)Input_IsKeyDown_Native);
        mono_add_internal_call("Loopie.InternalCalls::Input_IsKeyReleased", (void*)Input_IsKeyReleased_Native);
        mono_add_internal_call("Loopie.InternalCalls::Input_GetMousePosition", (void*)Input_GetMousePosition_Native);
        mono_add_internal_call("Loopie.InternalCalls::Input_GetMouseDelta", (void*)Input_GetMouseDelta_Native);
        mono_add_internal_call("Loopie.InternalCalls::Input_IsMouseButtonPressed", (void*)Input_IsMouseButtonPressed_Native);
    }
}