#pragma once

#include <string>
#include <memory>

namespace Loopie {

    class ScriptResource;

    class ScriptImporter {
    public:
        static std::shared_ptr<ScriptResource> Import(const std::string& path);
        static bool Compile(std::shared_ptr<ScriptResource> script);
    };

}