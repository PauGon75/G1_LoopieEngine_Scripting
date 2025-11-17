#pragma once 

#include "Editor/Interfaces/Interface.h"

#include <string>
#include <vector>

namespace Loopie {
	class EditorMenuInterface : public Interface {
	public:
		EditorMenuInterface();
		~EditorMenuInterface() = default;
		void Init() override {}
		void Render() override;

	private:
		void RenderAboutMenu();
		void RenderInfoConfigMenu();

		void RenderOpenProjectPopUp();
		void RenderCreateProjectPopUp();

	private:
		char m_projectName[128] = "";
		std::string m_projectPath = "";


		/// Menus States
		bool m_showAboutMenu = false;
		bool m_showInfoConfigMenu = false;


		/// Plot
		const int LOG_SIZE = 100;
		std::vector<float> m_fpsLog;
		std::vector<float> m_msLog;
	};
}