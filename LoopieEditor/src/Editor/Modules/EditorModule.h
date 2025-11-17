#pragma once

#include "Loopie/Core/Module.h"

#include "Editor/Interfaces/Workspace/InspectorInterface.h"
#include "Editor/Interfaces/Workspace/ConsoleInterface.h"
#include "Editor/Interfaces/Workspace/HierarchyInterface.h"
#include "Editor/Interfaces/Workspace/SceneInterface.h"
#include "Editor/Interfaces/Workspace/GameInterface.h"
#include "Editor/Interfaces/Workspace/EditorMenuInterface.h"
#include "Editor/Interfaces/Workspace/AssetsExplorerInterface.h"

namespace Loopie {

	class Camera;

	class EditorModule : public Module {
	public:
		EditorModule() = default;
		~EditorModule() = default;

		void OnLoad()override;
		void OnUnload()override;

		void OnUpdate(float dt) override;

		void OnInterfaceRender()override;
	private:
		void RenderWorld(Camera* camera);
		/// Test
		void CreateBakerHouse();
		///
	private:
		InspectorInterface m_inspector;
		ConsoleInterface m_console;
		HierarchyInterface m_hierarchy;
		SceneInterface m_scene;
		GameInterface m_game;
		EditorMenuInterface m_mainMenu;
		AssetsExplorerInterface m_assetsExplorer;

		Scene* scene;
	};
}