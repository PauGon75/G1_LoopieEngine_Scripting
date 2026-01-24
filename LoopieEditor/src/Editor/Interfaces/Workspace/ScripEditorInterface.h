#pragma once
#include "Editor/Interfaces/Interface.h"
#include <filesystem>
#include <string>

namespace Loopie {
    class ScriptEditorInterface : public Interface {
    public:
        ScriptEditorInterface();
        ~ScriptEditorInterface() = default;

        void Render() override;
        void OpenScript(const std::filesystem::path& path);

    private:
        void Save();

        std::filesystem::path m_currentAssetPath;
        char m_textBuffer[1024 * 64] = "";
        bool m_isOpen = false;
    };
}