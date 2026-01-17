#pragma once

#include "Loopie/Core/Module.h"

#include <mono/jit/jit.h>

namespace Loopie {

    class ScriptingModule : public Module {
    public:
        ScriptingModule() : Module("Scripting") {}

        void OnLoad() override;
        void OnUnload() override;

    private:
        MonoDomain* m_rootDomain = nullptr;
    };

}