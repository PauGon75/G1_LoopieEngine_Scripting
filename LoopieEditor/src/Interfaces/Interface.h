#pragma once

#include "Loopie/Core/Application.h"

#include <imgui.h>

namespace Loopie {
	class Interface {
	public:
		//Interface(ImVec2 wPosition = { 0,0 }, ImVec2 wSize = { 0,0 }) : windowPosition(wPosition), windowSize(wSize) {};
		Interface() = default;
		~Interface() = default;
		/*virtual void OnLoad()
		{
			windowPosition = { Application::GetInstance().GetWindow()->GetPosition().first, Application::GetInstance().GetWindow()->GetPosition().second };
			windowSize = { Application::GetInstance().GetWindow()->GetSize().first, Application::GetInstance().GetWindow()->GetSize().second };
		};*/
		virtual void Render() {};
	protected:
		/*ImVec2 windowPosition = { 0,0 };
		ImVec2 windowSize = { 0,0 };*/
	};
}