#include "EditorModule.h"

#include "Loopie/Core/Application.h"

//// Test
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Files/MeshImporter.h"

#include "Loopie/Core/Math.h"
#include "Loopie/Resources/ResourceDatabase.h"
#include "Loopie/Resources/AssetRegistry.h"
///


#include <imgui.h>

namespace Loopie
{
	void EditorModule::OnLoad()
	{
		AssetRegistry::Initialize();
		Application::GetInstance().GetWindow().SetResizable(true);



		cameraT.SetPosition({ 0,0,-50.f });
		camera = std::make_shared<Camera>(cameraT);
	}

	void EditorModule::OnUnload()
	{
		AssetRegistry::Shutdown();
		for (size_t i = 0; i < meshRenderers.size(); i++)
		{
			delete meshRenderers[i];
		}
		meshRenderers.clear();
	}

	void EditorModule::OnUpdate(float dt)
	{
		InputEventManager& inputEvent = Application::GetInstance().GetInputEvent();

		
		if (inputEvent.HasEvent(SDL_EVENT_WINDOW_RESIZED)) {
			ivec2 windowSize = Application::GetInstance().GetWindow().GetSize();
			camera->SetViewport(0, 0, windowSize.x, windowSize.y);
		}

		if (inputEvent.HasFileBeenDropped()) { //// Move this to an AssetInspectorClass
			const char* fileName = inputEvent.GetDroppedFile(0);
			if (MeshImporter::CheckIfIsModel(fileName)) {

				if (!AssetRegistry::AssetExists(fileName)) {
					std::vector<std::string> cacheFiles = MeshImporter::LoadModel(fileName);

					for (size_t i = 0; i < cacheFiles.size(); i++)
					{
						std::filesystem::path path = cacheFiles[i];

						AssetMetadata metadata;
						metadata.uuid = UUID(path.stem().string());
						metadata.cachePath = cacheFiles[i];
						metadata.sourcePath = fileName;
						AssetRegistry::RegisterAsset(metadata);

						std::shared_ptr<Mesh> mesh = ResourceDatabase::LoadResource<Mesh>(metadata.uuid);
						if (mesh)
							meshRenderers.push_back(new MeshRenderer(mesh));
					}
				}
				else {
					std::vector<UUID> uuids = AssetRegistry::GetUUIDFromSourcePath(fileName);
					for (size_t i = 0; i < uuids.size(); i++)
					{
						AssetMetadata* metadata = AssetRegistry::GetMetadata(uuids[i]);
						std::shared_ptr<Mesh> mesh = ResourceDatabase::LoadResource<Mesh>(metadata->uuid);
						if (mesh)
							meshRenderers.push_back(new MeshRenderer(mesh));

					}
				}
			}
		}

		vec3 moveCameraInput = { 0,0,0 };
		if (inputEvent.GetKeyStatus(SDL_SCANCODE_W) == KeyState::REPEAT)
			moveCameraInput.z += 1;
		if (inputEvent.GetKeyStatus(SDL_SCANCODE_S) == KeyState::REPEAT)
			moveCameraInput.z -= 1;
		if (inputEvent.GetKeyStatus(SDL_SCANCODE_A) == KeyState::REPEAT)
			moveCameraInput.x += 1;
		if (inputEvent.GetKeyStatus(SDL_SCANCODE_D) == KeyState::REPEAT)
			moveCameraInput.x -= 1;

		cameraT.Translate(moveCameraInput * 10.f * dt);
		rotation = SPEED * dt;
		meshT.Rotate({ 0,rotation,0 });
		glm::mat4 modelViewProj = camera->GetViewProjectionMatrix() * meshT.GetTransformMatrix();

		for (size_t i = 0; i < meshRenderers.size(); i++)
		{
			meshRenderers[i]->GetShader().Bind();
			meshRenderers[i]->GetShader().SetUniformMat4("modelViewProj", modelViewProj);
			meshRenderers[i]->Render();
		}
	}

	void EditorModule::OnInterfaceRender()
	{
		ImGui::DockSpaceOverViewport();

		m_mainMenu.Render();
		m_inspector.Render();
		m_console.Render();
		m_hierarchy.Render();
		m_scene.Render();
	}
}