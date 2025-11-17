#pragma once

#include "Loopie/Core/Application.h"

#include <imgui.h>

namespace Loopie {
	class Interface {
	public:
		Interface() = default;
		~Interface() = default;
		virtual void Update(float dt, const InputEventManager& inputEvent) {};
		virtual void Render() {};
		virtual void Init() {};

	protected:
		bool m_focused = false;
	};
}