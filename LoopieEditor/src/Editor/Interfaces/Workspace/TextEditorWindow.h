#pragma once
#include "TextEditor/TextEditor.h"
#include <string>
#include <memory>

namespace Loopie
{
    class TextEditorWindow
    {
    public:
        TextEditorWindow();
        ~TextEditorWindow() = default;

        void Init();
        void Render();
        void Update();

        void OpenFile(const std::string& filepath);
        void SaveFile();
        void SaveFileAs(const std::string& filepath);
        void NewFile();
        void CloseFile();

        bool IsVisible() const { return m_isVisible; }
        void SetVisible(bool visible) { m_isVisible = visible; }
        bool HasUnsavedChanges() const { return m_hasUnsavedChanges; }

        void SetLanguage(const std::string& extension);

    private:
        ::TextEditor m_editor;
        std::string m_currentFile;
        std::string m_currentFilename;
        bool m_isVisible;
        bool m_hasUnsavedChanges;
        bool m_shouldFocus;

        void UpdateWindowTitle();
        void HandleShortcuts();
    };
}