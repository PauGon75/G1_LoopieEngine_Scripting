#include "EditorModule.h"

#include "Loopie/Core/Application.h"

//// Test
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Importers/MeshImporter.h"
#include "Loopie/Importers/TextureImporter.h"

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

		/////SCENE
		Application::GetInstance().CreateScene(""); /// Maybe default One
		scene = &Application::GetInstance().GetScene();

		cameraEntity = scene->CreateEntity("Camera");
		camera = cameraEntity->AddComponent<Camera>();
		camera->GetTransform()->SetPosition({ 0,0,-50.f });

		meshContainerEntity = scene->CreateEntity("ModelContainer");
		////
	
		ivec2 windowSize = Application::GetInstance().GetWindow().GetSize();
		camera->SetViewport(0, 0, windowSize.x, windowSize.y);

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

		
		if (inputEvent.HasEvent(SDL_EVENT_WINDOW_RESIZED)) {
			ivec2 windowSize = Application::GetInstance().GetWindow().GetSize();
			camera->SetViewport(0, 0, windowSize.x, windowSize.y);
		}
		if (inputEvent.GetKeyStatus(SDL_SCANCODE_I) == KeyState::DOWN) {
			app.SetInterfaceState(!app.IsInterfaceVisible());
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
						if (mesh) {
							std::shared_ptr<Entity> newEntity = scene->CreateEntity("ModelEntity");
							newEntity->SetParent(meshContainerEntity);
							MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();
							renderer->SetMesh(mesh);
						}
					}
				}
				else {
					std::vector<UUID> uuids = AssetRegistry::GetUUIDFromSourcePath(fileName);
					for (size_t i = 0; i < uuids.size(); i++)
					{
						AssetMetadata* metadata = AssetRegistry::GetMetadata(uuids[i]);
						std::shared_ptr<Mesh> mesh = ResourceDatabase::LoadResource<Mesh>(metadata->uuid);
						if (mesh) {
							std::shared_ptr<Entity> newEntity = scene->CreateEntity("ModelEntity");
							newEntity->SetParent(meshContainerEntity);
							MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();
							renderer->SetMesh(mesh);
						}
					}
				}
			}
			else if(TextureImporter::CheckIfIsImage(fileName)) {
				if (!AssetRegistry::AssetExists(fileName)) {
					std::string cacheFile = TextureImporter::LoadImage(fileName);
					std::filesystem::path path = cacheFile;

					AssetMetadata metadata;
					metadata.uuid = UUID(path.stem().string());
					metadata.cachePath = cacheFile;
					metadata.sourcePath = fileName;
					AssetRegistry::RegisterAsset(metadata);

					std::shared_ptr<Texture> texture = ResourceDatabase::LoadResource<Texture>(metadata.uuid);
					if (texture) {
						for (const auto& entity : meshContainerEntity->GetChildren())
						{
							MeshRenderer* renderer = entity->GetComponent<MeshRenderer>();
							if (renderer) {
								renderer->SetTexture(texture);
							}
						}
					}
				}
				else {
					std::vector<UUID> uuids = AssetRegistry::GetUUIDFromSourcePath(fileName);
					for (size_t i = 0; i < uuids.size(); i++)
					{
						AssetMetadata* metadata = AssetRegistry::GetMetadata(uuids[i]);
						std::shared_ptr<Texture> texture = ResourceDatabase::LoadResource<Texture>(metadata->uuid);
						if (texture) {
							for (const auto& entity : meshContainerEntity->GetChildren())
							{
								MeshRenderer* renderer = entity->GetComponent<MeshRenderer>();
								if (renderer) {
									renderer->SetTexture(texture);
								}
							}
						}
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

		camera->GetTransform()->Translate(moveCameraInput * 10.f * dt);
		rotation = SPEED * dt;
		//meshContainerEntity->GetTransform()->Rotate({0,rotation,0}); //// this should Propagete to its childs

		const matrix4& viewProj = camera->GetViewProjectionMatrix();
		for (auto& entity : scene->GetAllEntities()) {
			MeshRenderer* renderer = entity.second->GetComponent<MeshRenderer>();
			if (renderer) {
				renderer->GetTransform()->Rotate({ 0,rotation,0 }); //// this should Propagete to its childs
				glm::mat4 modelViewProj = viewProj * entity.second->GetTransform()->GetTransformMatrix();
				renderer->GetShader().Bind();
				renderer->GetShader().SetUniformMat4("modelViewProj", modelViewProj);
				renderer->Render();
			}
		}
	}

	void EditorModule::OnInterfaceRender()
	{
		ImGui::DockSpaceOverViewport();

		m_mainMenu.Render();
		m_inspector.Render();
		m_console.Render();
		m_hierarchy.Render();
		m_assetsExplorer.Render();
		m_scene.Render();
	}
}