#include "EditorModule.h"

#include "Loopie/Core/Application.h"

//// Test
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Render/Gizmo.h"

#include "Loopie/Core/Math.h"

#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Importers/TextureImporter.h"

#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Transform.h"
///


#include <imgui.h>

namespace Loopie
{
	void EditorModule::OnLoad()
	{
		AssetRegistry::Initialize();
		Application::GetInstance().GetWindow().SetResizable(true);

		std::string defaultTeturePath = "assets/textures/simpleWhiteTexture.png";
		Metadata& meta = AssetRegistry::GetOrCreateMetadata(defaultTeturePath);
		TextureImporter::ImportImage(defaultTeturePath, meta);
		Renderer::SetDefaultTexture(ResourceManager::GetTexture(meta));

		/////SCENE
		Application::GetInstance().CreateScene(""); /// Maybe default One
		scene = &Application::GetInstance().GetScene();
		CreateBakerHouse();

		scene->CreateEntity({ 0,0,-10 }, { 1,0,0,0 }, {1,1,1}, nullptr, "MainCamera")->AddComponent<Camera>();
		scene->CreateEntity({ 0,0,-20 }, { 1,0,0,0 }, {1,1,1}, nullptr, "SecondaryCamera")->AddComponent<Camera>();
		////

		m_assetsExplorer.Init();
		m_console.Init();
		m_hierarchy.Init();
		m_inspector.Init();
		m_game.Init();
		m_scene.Init();
		m_mainMenu.Init();

		m_hierarchy.SetScene(scene);

	}

	void EditorModule::OnUnload()
	{
		AssetRegistry::Shutdown();
	}

	void EditorModule::OnUpdate(float dt)
	{
		Application& app = Application::GetInstance();
		InputEventManager& inputEvent = app.GetInputEvent();

		if (inputEvent.HasEvent(SDL_EVENT_WINDOW_FOCUS_GAINED)) {
			AssetRegistry::RefreshAssetRegistry();
		}

		m_hierarchy.Update(dt, inputEvent);
		m_assetsExplorer.Update(dt, inputEvent);
		m_scene.Update(dt, inputEvent);

		const std::vector<Camera*>& cameras = Renderer::GetRendererCameras();
		for (const auto cam : cameras)
		{
			std::shared_ptr<FrameBuffer> buffer = cam->GetRenderTarget();
			if (buffer)
			{
				buffer->Bind();
				buffer->Clear();
				buffer->Unbind();
			}
		}

		/// RenderToTarget
		for (const auto cam : cameras)
		{
			if (!cam->GetIsActive())
				continue;

			std::shared_ptr<FrameBuffer> buffer = cam->GetRenderTarget();
			if (!buffer)
				continue;

			Renderer::BeginScene(cam->GetViewMatrix(), cam->GetProjectionMatrix());
			Renderer::SetViewport(0, 0, buffer->GetWidth(), buffer->GetWidth());
			buffer->Bind();
			RenderWorld(cam);
			Renderer::EndScene();

			if (buffer) {
				buffer->Unbind();
			}
		}

		/// SceneWindowRender
		m_scene.StartScene();
		Renderer::BeginScene(m_scene.GetCamera()->GetViewMatrix(), m_scene.GetCamera()->GetProjectionMatrix());
		RenderWorld(m_scene.GetCamera());
		Renderer::EndScene();
		m_scene.EndScene();

		/// GameWindowRender
		m_game.StartScene();
		if (m_game.GetCamera() && m_game.GetCamera()->GetIsActive()) {
			Renderer::BeginScene(m_game.GetCamera()->GetViewMatrix(), m_game.GetCamera()->GetProjectionMatrix(), false);
			RenderWorld(m_game.GetCamera());
			Renderer::EndScene();
		}
		m_game.EndScene();
	}

	void EditorModule::OnInterfaceRender()
	{

		ImGui::DockSpaceOverViewport();

		m_mainMenu.Render();
		m_inspector.Render();
		m_console.Render();
		m_hierarchy.Render();
		m_assetsExplorer.Render();
		m_game.Render();
		m_scene.Render();
	}

	void EditorModule::RenderWorld(Camera* camera)
	{
		for (auto& [uuid, entity] : scene->GetAllEntities()) {
			if (!entity->GetIsActive())
				continue;
			MeshRenderer* renderer = entity->GetComponent<MeshRenderer>();
			if (!renderer || !renderer->GetIsActive())
				continue;
			Renderer::AddRenderItem(renderer->GetMesh()->GetVAO(), renderer->GetMaterial(), entity->GetTransform());
		}
	}

	void EditorModule::CreateBakerHouse()
	{
		m_scene.ChargeModel("assets/models/BakerHouse.fbx");
		m_scene.ChargeTexture("assets/textures/Baker_house.png");
	}	
}