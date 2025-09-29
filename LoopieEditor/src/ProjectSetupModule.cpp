#include "ProjectSetupModule.h"

#include "EditorModule.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Project/Project.h"

#include <imgui.h>


namespace Loopie
{
	void ProjectSetupModule::OnUpdate()
	{
		Application& app = Application::GetInstance();
		if (!app.m_activeProject.IsEmpty())
		{
			app.AddModule(new EditorModule());
			app.RemoveModule(this);
		}
	}
	void ProjectSetupModule::OnLoad()
	{
		m_interface = new ProjectSetupInterface();
	}
	void ProjectSetupModule::OnUnload()
	{
		delete m_interface;
	}
	void ProjectSetupModule::OnInterfaceRender()
	{
		ImGui::DockSpaceOverViewport();
		m_interface->Render();
	}
}