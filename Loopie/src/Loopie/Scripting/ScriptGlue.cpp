#include "ScriptGlue.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Scene/Scene.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h" 
#include "Loopie/Core/Log.h"

#include <mono/jit/jit.h>

namespace Loopie {


    static void NativeLog(MonoString* string) {
      
        char* cStr = mono_string_to_utf8(string);
      
        Log::Info("[Script]: {0}", cStr);
     
        mono_free(cStr);
    }

    static void Native_SetTranslation(uint64_t entityID, float x, float y, float z) {
       
        Scene& scene = Application::GetInstance().GetScene();

       
        /*std::shared_ptr<Entity> entity = scene.GetEntity(UUID(entityID));

        if (entity) {
          
            Transform* transform = entity->GetTransform();
            if (transform) {
      
              
            }
        }*/
    }

    static void Native_CreateEntity(MonoString* name) {
        char* cStr = mono_string_to_utf8(name);
    
        Application::GetInstance().GetScene().CreateEntity(std::string(cStr));
        mono_free(cStr);
    }

    void ScriptGlue::RegisterGlue() {


        mono_add_internal_call("Loopie.InternalCalls::NativeLog", (void*)NativeLog);
        mono_add_internal_call("Loopie.InternalCalls::Native_SetTranslation", (void*)Native_SetTranslation);
        mono_add_internal_call("Loopie.InternalCalls::Native_CreateEntity", (void*)Native_CreateEntity);
    }
}