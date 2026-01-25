#include "ScriptEditorInterface.h"
#include "Loopie/Scripting/ScriptingModule.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include <imgui.h>
#include <fstream>
#include <sstream>

namespace Loopie {
    ScriptEditorInterface::ScriptEditorInterface() {
        m_focused = false;
    }

    void ScriptEditorInterface::OpenScript(const std::filesystem::path& path) {
        m_currentAssetPath = path;

        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();

        std::string content = buffer.str();
        memset(m_textBuffer, 0, sizeof(m_textBuffer));
        strncpy(m_textBuffer, content.c_str(), sizeof(m_textBuffer) - 1);

        m_isOpen = true;
    }

    void ScriptEditorInterface::Render() {
        if (!m_isOpen) return;

        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Script Live Editor", &m_isOpen, ImGuiWindowFlags_MenuBar)) {
            m_focused = ImGui::IsWindowFocused();

            if (ImGui::BeginMenuBar()) {
                if (ImGui::MenuItem("Save & Compile (Ctrl+S)")) Save();
                ImGui::EndMenuBar();
            }

            if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) Save();

            ImGui::TextDisabled("Archivo: %s", m_currentAssetPath.string().c_str());

            ImGui::InputTextMultiline("##editor", m_textBuffer, sizeof(m_textBuffer),
                ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput);
        }
        ImGui::End();
    }

    void ScriptEditorInterface::Save() {
        if (m_currentAssetPath.empty()) return;

        std::ofstream assetOut(m_currentAssetPath);
        assetOut << m_textBuffer;
        assetOut.close();

        std::string fileName = m_currentAssetPath.filename().string();

        std::filesystem::path corePath ="../../../LoopieScriptCore/" + fileName;

        std::ofstream coreOut(corePath);
        coreOut << m_textBuffer;
        coreOut.close();

        Log::Info("ScriptEditor: Cambios guardados y sincronizados.");

        ScriptingModule::ReloadAssembly();
    }
}