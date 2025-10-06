#include "EditorModule.h"

#include "Loopie/Core/Application.h"

#include <imgui.h>

namespace Loopie
{
	void EditorModule::OnLoad()
	{
		Application::GetInstance().GetWindow()->SetResizable(true);
	}
	void EditorModule::OnUnload()
	{

		
	}
	void EditorModule::OnInterfaceRender()
	{
		ImGui::DockSpaceOverViewport();

		m_mainMenu.Render();
		m_inspector.Render();
		m_console.Render();
		m_hierarchy.Render();
		m_scene.Render();
	}
}