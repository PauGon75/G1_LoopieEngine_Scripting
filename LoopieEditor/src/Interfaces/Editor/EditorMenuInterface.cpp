#include "EditorMenuInterface.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Files/FileDialog.h"

#include <imgui.h>
#include <SDL3/SDL_misc.h>

namespace Loopie {
	EditorMenuInterface::EditorMenuInterface() {

	}

	void EditorMenuInterface::Render() {

		ImGuiID openProjectPopUpId = ImGui::GetID("###OpenProjectPopUp");
		ImGuiID createProjectPopUpId = ImGui::GetID("###CreateProjectPopUp");

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open"))
				{
					ImGui::OpenPopup(openProjectPopUpId);
					m_projectPath = "";
				}

				if (ImGui::MenuItem("New"))
				{
					ImGui::OpenPopup(createProjectPopUpId);
					m_projectPath = "";
					m_projectName[0] = '\0';
				}

				if (ImGui::MenuItem("Exit"))
					Application::GetInstance().Close();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Disable"))
				{
					Application::GetInstance().SetInterfaceState(false);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Documentation"))
				{
					SDL_OpenURL("https://github.com/UPC-GameEngines-BCN-2025/LoopieEngine/docs");
				}

				if (ImGui::MenuItem("Report"))
				{
					SDL_OpenURL("https://github.com/UPC-GameEngines-BCN-2025/LoopieEngine/issues");
				}

				if (ImGui::MenuItem("Release"))
				{
					SDL_OpenURL("https://github.com/UPC-GameEngines-BCN-2025/LoopieEngine/releases");
				}

				if (ImGui::MenuItem("About"))
				{
					m_showAboutMenu = true;
				}
				ImGui::EndMenu();
			}


			ImGui::EndMainMenuBar();
		}

		if (m_showAboutMenu)
			RenderAboutMenu();

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		RenderOpenProjectPopUp();

		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		RenderCreateProjectPopUp();
		
	}

	void EditorMenuInterface::RenderAboutMenu()
	{

		ImGui::Begin("About Loopie Engine", &m_showAboutMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

		ImGui::Text("Loopie Engine v0.0.1");
		ImGui::Separator();
		ImGui::Text("Developed by:");
		ImGui::BulletText("Ana Alcazar");
		ImGui::BulletText("Pol Sierra");
		ImGui::BulletText("Adria Garcia");

		ImGui::Separator();
		ImGui::Text("Built using:");
		ImGui::BulletText("SDL3 / OpenGl / Glm");
		ImGui::BulletText("Dear ImGui / ImGuizmo");
		ImGui::BulletText("Assimp / Spdlog");
		ImGui::BulletText("nlohmann-json / Nativefiledialog-Extended");

		ImGui::Separator();
		ImGui::BeginChild("LicenseText",{0,150});
		ImGui::TextWrapped(
			"MIT License\n"
			"Copyright (c) 2025 CITM - UPC\n\n"
			"Permission is hereby granted, free of charge, to any person obtaining a copy "
			"of this software and associated documentation files (the \"Software\"), to deal "
			"in the Software without restriction, including without limitation the rights "
			"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
			"copies of the Software, and to permit persons to whom the Software is "
			"furnished to do so, subject to the following conditions:\n\n"
			"The above copyright notice and this permission notice shall be included in all "
			"copies or substantial portions of the Software.\n\n"
			"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
			"SOFTWARE.");
		ImGui::EndChild();

		ImGui::End();
	}

	void EditorMenuInterface::RenderOpenProjectPopUp()
	{
		if (ImGui::BeginPopupModal("Open Project###OpenProjectPopUp", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
			///Render interface
			ImGui::InputText("Path", m_projectPath.data(), m_projectPath.capacity(), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("##", { 20,20 }))
			{
				DialogResult result = FileDialog::SelectFolder();
				if (result.Status == DialogResultType::SUCCESS)
				{
					m_projectPath = result.Paths[0].string();
				}
			}

			if (ImGui::Button("Open Project", { 150,20 }))
			{
				if (Application::GetInstance().m_activeProject.Open(m_projectPath)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void EditorMenuInterface::RenderCreateProjectPopUp() {
		if (ImGui::BeginPopupModal("Create Project###CreateProjectPopUp", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
			///Render interface

			ImGui::InputText("Project Name", m_projectName, IM_ARRAYSIZE(m_projectName));

			ImGui::InputText("Path", m_projectPath.data(), m_projectPath.capacity(), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("##", { 20,20 }))
			{
				DialogResult result = FileDialog::SelectFolder();
				if (result.Status == DialogResultType::SUCCESS)
				{
					m_projectPath = result.Paths[0].string();
				}
			}

			if (ImGui::Button("Create Project", { 150,20 }))
			{
				if (Application::GetInstance().m_activeProject.Create(m_projectPath, m_projectName)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}