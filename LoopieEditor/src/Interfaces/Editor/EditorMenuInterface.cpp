#include "EditorMenuInterface.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Files/FileDialog.h"

#include <imgui.h>

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
				}

				if (ImGui::MenuItem("New"))
				{
					ImGui::OpenPopup(createProjectPopUpId);
				}

				if (ImGui::MenuItem("Exit"))
					Application::GetInstance().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}


		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		RenderOpenProjectPopUp();

		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		RenderCreateProjectPopUp();
		
	}

	void EditorMenuInterface::RenderOpenProjectPopUp()
	{
		if (ImGui::BeginPopupModal("Open Project###OpenProjectPopUp", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
			///Render interface
			static std::string projectPath;
			ImGui::InputText("Path", projectPath.data(), projectPath.capacity(), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("##", { 20,20 }))
			{
				DialogResult result = FileDialog::SelectFolder();
				if (result.Status == DialogResultType::SUCCESS)
				{
					projectPath = result.Paths[0].string();
				}
			}

			if (ImGui::Button("Open Project", { 150,20 }))
			{
				if (Application::GetInstance().m_activeProject.Open(projectPath)) {
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

			static char projectName[128];
			static std::string projectPath;

			ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));

			ImGui::InputText("Path", projectPath.data(), projectPath.capacity(), ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("##", { 20,20 }))
			{
				DialogResult result = FileDialog::SelectFolder();
				if (result.Status == DialogResultType::SUCCESS)
				{
					projectPath = result.Paths[0].string();
				}
			}

			if (ImGui::Button("Create Project", { 150,20 }))
			{
				if (Application::GetInstance().m_activeProject.Create(projectPath, projectName)) {
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