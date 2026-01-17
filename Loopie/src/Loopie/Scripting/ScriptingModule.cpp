#include "ScriptingModule.h"
#include "Loopie/Core/Log.h"
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>

namespace Loopie {

    void ScriptingModule::OnLoad() {
        Log::Info("TEST MONO: Intentando inicializar...");

        // 1. Decimos a Mono dónde buscar sus piezas (usa rutas relativas al .exe)
        // Si lanzas desde el IDE, asegúrate de que 'vendor' sea accesible
        mono_set_dirs("vendor/mono/lib", "vendor/mono/etc");

        // 2. Configuración necesaria
        mono_config_parse(NULL);

        // 3. El momento de la verdad: Arrancar el JIT
        m_rootDomain = mono_jit_init("LoopieTestDomain");

        if (m_rootDomain) {
            Log::Info("******************************************");
            Log::Info("   ¡EXITO! MONO ESTA VIVO EN LOOPIE       ");
            Log::Info("******************************************");
        }
        else {
            Log::Error("!!! ERROR: Mono no pudo arrancar. Revisa carpetas vendor !!!");
        }
    }

    void ScriptingModule::OnUnload() {
        if (m_rootDomain) {
            mono_jit_cleanup(m_rootDomain);
            Log::Info("MONO: Limpieza finalizada.");
        }
    }
}