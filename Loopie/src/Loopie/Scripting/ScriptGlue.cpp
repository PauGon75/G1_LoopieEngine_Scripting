#include "ScriptGlue.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/object.h>

namespace Loopie {

    // Funciones nativas que C# puede llamar
    static void Log_Native(MonoString* message) {
        char* cStr = mono_string_to_utf8(message);
        Log::Info("[C#] {0}", cStr);
        mono_free(cStr);
    }

    static void LogWarning_Native(MonoString* message) {
        char* cStr = mono_string_to_utf8(message);
        Log::Warn("[C#] {0}", cStr);
        mono_free(cStr);
    }

    static void LogError_Native(MonoString* message) {
        char* cStr = mono_string_to_utf8(message);
        Log::Error("[C#] {0}", cStr);
        mono_free(cStr);
    }

    void ScriptGlue::RegisterGlue() {
        // Registrar las funciones para que C# pueda llamarlas
        mono_add_internal_call("Loopie.InternalCalls::Log", (void*)Log_Native);
        mono_add_internal_call("Loopie.InternalCalls::LogWarning", (void*)LogWarning_Native);
        mono_add_internal_call("Loopie.InternalCalls::LogError", (void*)LogError_Native);

        Log::Info("ScriptGlue: Funciones de logging registradas.");
    }
}