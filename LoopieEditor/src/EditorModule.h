#pragma once

#include "Loopie/Core/Module.h"

#include "Interfaces/Editor/InspectorInterface.h"
#include "Interfaces/Editor/ConsoleInterface.h"
#include "Interfaces/Editor/HierarchyInterface.h"
#include "Interfaces/Editor/SceneInterface.h"
#include "Interfaces/Editor/EditorMenuInterface.h"

/// Test

#include "Loopie/Core/Math.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Camera.h"
#include "Loopie/Components/Transform.h"

///

namespace Loopie {
	class EditorModule : public Module {
	public:
		EditorModule() = default;
		~EditorModule() = default;

		void OnLoad()override;
		void OnUnload()override;

		void OnUpdate(float dt) override;

		void OnInterfaceRender()override;
	private:
		InspectorInterface m_inspector;
		ConsoleInterface m_console;
		HierarchyInterface m_hierarchy;
		SceneInterface m_scene;
		EditorMenuInterface m_mainMenu;


		/// Test
		Transform cameraT;
		std::shared_ptr<Camera> camera; /// Create an especific CameraControllerClass????

		Transform meshT;
		std::vector<MeshRenderer*> meshRenderers;

		const float SPEED = 100.0f;
		float rotation = 0;
		///
	};
}