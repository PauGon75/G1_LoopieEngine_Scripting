#pragma once 

#include "src/Interfaces/Interface.h"

namespace Loopie {
	class EditorMenuInterface : public Interface {
	public:
		EditorMenuInterface();
		~EditorMenuInterface() = default;
		void Render() override;

	private:
		void RenderOpenProjectPopUp();
		void RenderCreateProjectPopUp();
	};
}