#include "ScriptImporter.h"
#include "ScriptResource.h"
#include "ScriptCompiler.h"
#include "Loopie/Core/Log.h"

namespace Loopie {

    std::shared_ptr<ScriptResource> ScriptImporter::Import(const std::string& path) {
        std::filesystem::path filePath(path);

        UUID scriptID;

        auto script = std::make_shared<ScriptResource>(scriptID, path);
        script->SetClassName(filePath.stem().string());
        script->SetLibraryPath("Library/Scripts/" + script->GetClassName() + ".dll");

        Compile(script);
        return script;
    }

    bool ScriptImporter::Compile(std::shared_ptr<ScriptResource> script) {
        CompileResult result = ScriptCompiler::Compile(script->GetSourcePath(), script->GetLibraryPath());

        if (result.Success) {
            script->SetCompiled(true);
            Log::Info("Compilacion exitosa: {0}", script->GetClassName());
            return true;
        }
        else {
            script->SetCompiled(false);
            Log::Error("Error en script {0}: {1}", script->GetClassName(), result.ErrorMessage);
            return false;
        }
    }
}