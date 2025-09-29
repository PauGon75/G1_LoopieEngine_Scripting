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
		//ImGui::SetNextWindowSize(ImVec2(windowSizeX/3,windowSizeY*5/6),ImGuiCond_Once);
		//ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Once);
		ImGui::Begin("Recent Projects", nullptr);
		
		ImGui::End();
#pragma endregion

#pragma region Load Project
		ImGui::Begin("Load Project", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::LabelText("Path", m_projectPath.string().c_str());
		ImGui::SameLine();
		if (ImGui::Button("##", { 20,20 }))
		{
			DialogResult result = FileDialog::SelectFolder();
			if (result.Status == DialogResultType::SUCCESS)
			{
				m_projectPath = result.Paths[0];
			}
		}
		ImGui::LabelText("##", m_projectPath.string().c_str());
		if (ImGui::Button("Load Project", { 150,20 }))
		{
			if (Application::GetInstance().m_activeProject.Open(m_projectPath)) {
				Log::Info("Project Selected");
			}
			else {
				Log::Info("Project Couldn't be Selected");
			}
		}
		ImGui::End();
#pragma endregion
#pragma region Create Project
		ImGui::Begin("Create Project", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::InputText("Project Name", m_projectName, IM_ARRAYSIZE(m_projectName));

		ImGui::LabelText("Path", m_projectPath.string().c_str());
		ImGui::SameLine();
		if (ImGui::Button("##", { 20,20 }))
		{
			DialogResult result = FileDialog::SelectFolder();
			if (result.Status == DialogResultType::SUCCESS)
			{
				m_projectPath = result.Paths[0];
			}
		}
		if (ImGui::Button("Create Project", { 150,20 }))
		{
			if (Application::GetInstance().m_activeProject.Create(m_projectPath, m_projectName)) {
				Log::Info("Project Created");
			}
			else {
				Log::Info("Project Couldn't be Created");
			}
		}
		ImGui::End();
#pragma endregion
	}
}