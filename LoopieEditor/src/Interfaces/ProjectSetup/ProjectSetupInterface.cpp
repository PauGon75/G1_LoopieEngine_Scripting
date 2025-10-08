#include "ProjectSetupInterface.h"

#include "Loopie/Files/FileDialog.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Window.h"

#include <imgui.h>
namespace Loopie
{
	ProjectSetupInterface::ProjectSetupInterface()
	{
		/*Application::GetInstance().GetWindow()->SetTitle("Loopie Engine | Project Setup");
		Application::GetInstance().GetWindow()->SetResizable(false);
		windowSizeX = Application::GetInstance().GetWindow()->GetSize().first;
		windowSizeY = Application::GetInstance().GetWindow()->GetSize().second;*/
	}

	void ProjectSetupInterface::Render()
	{


#pragma region Recent Projects
		ImGui::Begin("Recent Projects", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		
		ImGui::End();
#pragma endregion

#pragma region Load
		ImGui::Begin("Load Project", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::InputText("Path", m_loadProjectPath.data(), m_loadProjectPath.capacity(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("##", { 20,20 }))
		{
			DialogResult result = FileDialog::SelectFolder();
			if (result.Status == DialogResultType::SUCCESS)
			{
				m_loadProjectPath = result.Paths[0].string();
			}
		}

		if (ImGui::Button("Open Project", { 150,20 }))
		{
			if (Application::GetInstance().m_activeProject.Open(m_loadProjectPath)) {
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::End();
#pragma endregion
#pragma region Create Project
		ImGui::Begin("Create Project", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse  | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::InputText("Project Name", m_projectName, IM_ARRAYSIZE(m_projectName));

		ImGui::InputText("Path", m_createProjectPath.data(), m_createProjectPath.capacity(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("##", { 20,20 }))
		{
			DialogResult result = FileDialog::SelectFolder();
			if (result.Status == DialogResultType::SUCCESS)
			{
				m_createProjectPath = result.Paths[0].string();
			}
		}

		if (ImGui::Button("Create Project", { 150,20 }))
		{
			if (Application::GetInstance().m_activeProject.Create(m_createProjectPath, m_projectName)) {
				ImGui::CloseCurrentPopup();
			}
		}
		//Log::Info("{0}x{1}",Application::GetInstance().GetWindow()->GetSize().first, Application::GetInstance().GetWindow()->GetSize().second);
		ImGui::End();
#pragma endregion
	}
}

