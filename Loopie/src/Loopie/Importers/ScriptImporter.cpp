#include "ScriptImporter.h"
#include "ScriptResource.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Core/Log.h"
#include <cstdlib>
#include <filesystem>

namespace Loopie {

    std::shared_ptr<ScriptResource> ScriptImporter::Import(const std::string& path) {
        Metadata* metadata = AssetRegistry::GetMetadata(path);
        UUID id = metadata ? metadata->UUID : UUID();

        auto script = std::make_shared<ScriptResource>(id, path);
        script->SetClassName(std::filesystem::path(path).stem().string());
        script->SetLibraryPath("Library/Scripts/" + script->GetClassName() + ".dll");

        ResourceManager::AddResource(path, script);

        Compile(script);
        return script;
    }

    bool ScriptImporter::Compile(std::shared_ptr<ScriptResource> script) {
        if (!std::filesystem::exists("Library/Scripts")) {
            std::filesystem::create_directories("Library/Scripts");
        }

        std::string command = "csc -target:library -out:" + script->GetLibraryPath() + " " + script->GetSourcePath();

        int result = std::system(command.c_str());
        bool success = (result == 0);

        script->SetCompiled(success);

        if (!success) {
            Log::Error("Scripting: Error al compilar {0}", script->GetClassName());
        }
        else {
            Log::Info("Scripting: {0} compilado con exito", script->GetClassName());
        }

        return success;
    }
}