#pragma once
#include "Interface.h"

#include <string>
#include <filesystem>

namespace Loopie {
	class ProjectSetupInterface : public Interface {
	public:
		ProjectSetupInterface();
		~ProjectSetupInterface() = default;
		void Render()override;
	private:
		std::filesystem::path m_projectPath;
		char m_projectName[128] = "";
		int windowSizeX = 0;
		int windowSizeY = 0;
	};
}