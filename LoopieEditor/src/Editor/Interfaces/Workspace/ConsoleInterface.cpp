#include "ConsoleInterface.h"
#include "Loopie/Core/Log.h"

#include <imgui.h>

namespace Loopie {
	ConsoleInterface::ConsoleInterface() {
        
	}

    void ConsoleInterface::Init()
    {
        msgColor[0] = vec4(0.5f, 0.5f, 0.5f, 1.0f);
        msgColor[1] = vec4(0.0f, 1.0f, 1.0f, 1.0f);
        msgColor[2] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        msgColor[3] = vec4(1.0f, 1.0f, 0.0f, 1.0f);
        msgColor[4] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        msgColor[5] = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    }

	void ConsoleInterface::Render() {
		if (ImGui::Begin("Console")) {
            const auto& logs = Log::GetLogEntries();

            static ImGuiTextFilter filter;
            filter.Draw("Filter");
            ImGui::Separator();

            if (ImGui::Button("Clear")) {
                Log::Clear();
            }
            ImGui::SameLine();

            static bool autoScroll = true;
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::SameLine(ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - 100);

            int msgCount = (int)(logs.size());
            if (msgCount > 9999)
                ImGui::Text("Messages: 999+");
            else
                ImGui::Text("Messages: %d", msgCount);
            ImGui::Separator();

            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (ImGui::BeginChild("Console Logs", ImVec2(0,avail.y))) {
                for (const auto& entry : logs) {
                    if (filter.PassFilter(entry.msg.c_str())) {
                        vec4 color = msgColor[entry.level];
                        ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), "%s", entry.msg.c_str());
                    }
                }

                float scrollY = ImGui::GetScrollY();
                float scrollMaxY = ImGui::GetScrollMaxY();
                if (autoScroll && (scrollY >= scrollMaxY - 1.0f)) {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
		}
		ImGui::End();
	}
}