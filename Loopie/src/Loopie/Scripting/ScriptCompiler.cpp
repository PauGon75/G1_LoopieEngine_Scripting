#include "ScriptCompiler.h"
#include "Loopie/Core/Log.h"
#include <filesystem>
#include <cstdlib>
#include <fstream>

namespace Loopie {

    CompileResult ScriptCompiler::Compile(const std::string& sourcePath, const std::string& outputPath) {
        if (!std::filesystem::exists("Library/Scripts")) {
            std::filesystem::create_directories("Library/Scripts");
        }

        std::string command = "csc -target:library -out:" + outputPath + " " + sourcePath;
        command += " > build_log.txt 2>&1";

        int exitCode = std::system(command.c_str());

        if (exitCode == 0) {
            std::filesystem::remove("build_log.txt");
            return { true, "" };
        }
        else {
            std::ifstream logFile("build_log.txt");
            std::string errors((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
            logFile.close();
            return { false, errors };
        }
    }
}