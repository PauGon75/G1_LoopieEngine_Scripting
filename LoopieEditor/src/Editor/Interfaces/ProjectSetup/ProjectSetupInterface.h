#pragma once
#include  "Editor/Interfaces/Interface.h"
#include "Loopie/Files/Json.h"

#include <string>

namespace Loopie {
	class ProjectSetupInterface : public Interface {
	public:
		ProjectSetupInterface();
		~ProjectSetupInterface();
		void Render()override;

	private:

		void GetRecentProjects();
		bool ExistsRecentProject(const std::string& id);
		void DeleteRecentProject(const std::string& id);

		bool TryOpenProject(const std::string& path) const;

		void RenderRecentProjectsMenu();
		void RenderCreateProjectMenu();
		void RenderLoadProjectMenu();

	private:
		std::vector<std::string> m_recentProjects;

		std::string m_createProjectPath;
		std::string m_loadProjectPath;
		char m_projectName[128] = "";
		int windowSizeX = 0;
		int windowSizeY = 0;
	};
}