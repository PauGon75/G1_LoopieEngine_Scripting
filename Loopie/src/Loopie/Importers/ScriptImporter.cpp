#include "ScriptImporter.h"
#include "ScriptResource.h"
#include "Loopie/Core/Log.h"
#include <filesystem>

namespace Loopie {

    std::shared_ptr<ScriptResource> ScriptImporter::Import(const std::string& path) {
        std::filesystem::path filePath(path);
        std::string extension = filePath.extension().string();

        if (extension != ".cs" && extension != ".dll") {
            Log::Error("ScriptImporter: Formato no soportado: {0}", extension);
            return nullptr;
        }

        auto script = std::make_shared<ScriptResource>(path);
        script->SetClassName(filePath.stem().string());

        std::string libPath = "Library/Scripts/" + script->GetClassName() + ".dll";
        script->SetLibraryPath(libPath);

        Log::Info("ScriptImporter: Importado '{0}' correctamente.", filePath.filename().string());

        return script;
    }

    bool ScriptImporter::Compile(std::shared_ptr<ScriptResource> script) {
        Log::Info("ScriptImporter: Preparando compilacion para {0}...", script->GetClassName());
        return true;
    }

}