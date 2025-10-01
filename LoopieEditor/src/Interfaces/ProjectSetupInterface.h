#pragma once
#include "Interface.h"

#include <string>

namespace Loopie {
	class ProjectSetupInterface : public Interface {
	public:
		ProjectSetupInterface();
		~ProjectSetupInterface() = default;
		void Render()override;
	private:
		std::string m_createProjectPath;
		std::string m_loadProjectPath;
		char m_projectName[128] = "";
		int windowSizeX = 0;
		int windowSizeY = 0;
	};
}