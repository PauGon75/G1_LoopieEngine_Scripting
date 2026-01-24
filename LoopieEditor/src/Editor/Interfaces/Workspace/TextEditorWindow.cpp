#include "TextEditorWindow.h"
#include "Loopie/Core/Log.h"
#include <imgui.h>
#include <fstream>
#include <sstream>

namespace Loopie
{
    TextEditorWindow::TextEditorWindow()
        : m_isVisible(false)
        , m_hasUnsavedChanges(false)
        , m_shouldFocus(false)
    {
    }

    void TextEditorWindow::Init()
    {
        m_editor.SetShowWhitespaces(false);
        m_editor.SetTabSize(4);

        auto palette = ::TextEditor::GetDarkPalette();
        m_editor.SetPalette(palette);

        m_editor.SetLanguageDefinition(::TextEditor::LanguageDefinition::CSharp());
    }

    void TextEditorWindow::Render()
    {
        if (!m_isVisible)
            return;

        std::string title = m_currentFilename.empty() ? "Text Editor" : m_currentFilename;
        if (m_hasUnsavedChanges)
            title += "*";

        if (m_shouldFocus)
        {
            ImGui::SetNextWindowFocus();
            m_shouldFocus = false;
        }

        ImGui::Begin(title.c_str(), &m_isVisible, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewFile();
                if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, !m_currentFile.empty()))
                    SaveFile();
                ImGui::Separator();
                if (ImGui::MenuItem("Close"))
                    CloseFile();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                bool hasUndo = m_editor.CanUndo();
                bool hasRedo = m_editor.CanRedo();
                bool hasSelection = m_editor.HasSelection();

                if (ImGui::MenuItem("Undo", "Ctrl+Z", nullptr, hasUndo))
                    m_editor.Undo();
                if (ImGui::MenuItem("Redo", "Ctrl+Y", nullptr, hasRedo))
                    m_editor.Redo();
                ImGui::Separator();
                if (ImGui::MenuItem("Copy", "Ctrl+C", nullptr, hasSelection))
                    m_editor.Copy();
                if (ImGui::MenuItem("Cut", "Ctrl+X", nullptr, hasSelection))
                    m_editor.Cut();
                if (ImGui::MenuItem("Paste", "Ctrl+V"))
                    m_editor.Paste();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // El editor necesita TODO el espacio disponible
        m_editor.Render("##TextEditorContent");

        if (m_editor.IsTextChanged())
            m_hasUnsavedChanges = true;

        ImGui::End();
    }

    void TextEditorWindow::Update()
    {
        if (!m_isVisible)
            return;

        HandleShortcuts();
    }

    void TextEditorWindow::OpenFile(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            Log::Error("Failed to open file: {0}", filepath);
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        m_editor.SetText(buffer.str());
        m_currentFile = filepath;

        size_t lastSlash = filepath.find_last_of("/\\");
        m_currentFilename = (lastSlash != std::string::npos)
            ? filepath.substr(lastSlash + 1)
            : filepath;

        size_t lastDot = m_currentFilename.find_last_of('.');
        if (lastDot != std::string::npos)
        {
            std::string extension = m_currentFilename.substr(lastDot);
            SetLanguage(extension);
        }

        m_hasUnsavedChanges = false;
        m_isVisible = true;
        m_shouldFocus = true;
        SDL_StartTextInput(Application::GetInstance().GetWindow().GetSDLWindow());
    }

    void TextEditorWindow::SaveFile()
    {
        if (m_currentFile.empty())
        {
            // Debería abrir save as dialog
            return;
        }

        std::ofstream file(m_currentFile);
        if (!file.is_open())
        {
            Log::Error("Failed to save file: {0}", m_currentFile);
            return;
        }

        file << m_editor.GetText();
        m_hasUnsavedChanges = false;
    }

    void TextEditorWindow::SaveFileAs(const std::string& filepath)
    {
        m_currentFile = filepath;

        size_t lastSlash = filepath.find_last_of("/\\");
        m_currentFilename = (lastSlash != std::string::npos)
            ? filepath.substr(lastSlash + 1)
            : filepath;

        SaveFile();
    }

    void TextEditorWindow::NewFile()
    {
        if (m_hasUnsavedChanges)
        {
            // Aquí deberías mostrar un diálogo de confirmación
        }

        m_editor.SetText("");
        m_currentFile.clear();
        m_currentFilename = "Untitled";
        m_hasUnsavedChanges = false;
        m_isVisible = true;
    }

    void TextEditorWindow::CloseFile()
    {
        if (m_hasUnsavedChanges)
        {
            // Diálogo de confirmación
        }

        m_isVisible = false;
        SDL_StopTextInput(Application::GetInstance().GetWindow().GetSDLWindow());
    }

    void TextEditorWindow::SetLanguage(const std::string& extension)
    {
        if (extension == ".cpp" || extension == ".h")
            m_editor.SetLanguageDefinition(::TextEditor::LanguageDefinition::CPlusPlus());
        else if (extension == ".cs")
            m_editor.SetLanguageDefinition(::TextEditor::LanguageDefinition::CSharp());
    }

    void TextEditorWindow::HandleShortcuts()
    {

        ImGuiIO& io = ImGui::GetIO();
        bool ctrl = io.KeyCtrl;
        bool shift = io.KeyShift;

        if (ctrl && !shift && ImGui::IsKeyPressed(ImGuiKey_S))
            SaveFile();
        else if (ctrl && shift && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            // Save As
        }
        else if (ctrl && ImGui::IsKeyPressed(ImGuiKey_N))
            NewFile();
    }
}