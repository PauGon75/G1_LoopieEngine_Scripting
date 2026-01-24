#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Loopie {
    class ScriptGlue {
    public:
        static void RegisterGlue();
    };
}