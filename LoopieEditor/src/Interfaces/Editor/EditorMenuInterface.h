#pragma once 

#include "src/Interfaces/Interface.h"

#include <string>

namespace Loopie {
	class EditorMenuInterface : public Interface {
	public:
		EditorMenuInterface();
		~EditorMenuInterface() = default;
		void Render() override;

	private:
		void RenderAboutMenu();

		void RenderOpenProjectPopUp();
		void RenderCreateProjectPopUp();

	private:
		char m_projectName[128] = "";
		std::string m_projectPath = "";

		bool m_showAboutMenu = false;
	};
}