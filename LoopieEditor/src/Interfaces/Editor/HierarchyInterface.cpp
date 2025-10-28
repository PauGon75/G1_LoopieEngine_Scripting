#include "HierarchyInterface.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Importers/MeshImporter.h"

#include <imgui.h>

namespace Loopie {
	std::shared_ptr<Entity> HierarchyInterface::s_SelectedEntity = nullptr;

	HierarchyInterface::HierarchyInterface() {
		
	}

	void HierarchyInterface::Render() {

		if (ImGui::Begin("Hierarchy")) {
			if (!m_scene) {
				ImGui::End();
				return;
			}
			for (const auto& entity : m_scene->GetRootEntity()->GetChildren())
			{
				DrawEntitySlot(entity);
			}
			
			if (ImGui::BeginPopupContextWindow("HierarchyBackgroundContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
				DrawContextMenu(nullptr);
				ImGui::EndPopup();
			}
			HotKeysSelectedEntiy();
		}
		ImGui::End();
	}

	void HierarchyInterface::SetScene(Scene* scene)
	{
		m_scene = scene;
	}

	void HierarchyInterface::DrawEntitySlot(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
			return;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		const auto& children = entity->GetChildren();
		bool hasChildren = !children.empty();

		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		if (s_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.get(), flags, entity->GetName().c_str());

		if (ImGui::IsItemClicked())
		{
			////Expand To Select Multiple
			s_SelectedEntity = entity;
		}

		if (ImGui::BeginPopupContextItem())
		{
			DrawContextMenu(entity);
			ImGui::EndPopup();
		}

		if (opened && hasChildren)
		{
			for (const auto& child : children)
			{
				DrawEntitySlot(child);
			}
			ImGui::TreePop();
		}

		
		
	}

	void HierarchyInterface::DrawContextMenu(const std::shared_ptr<Entity>& entity)
	{

		if (ImGui::MenuItem("Create Empty"))
		{
			std::shared_ptr<Entity> newEntity = m_scene->CreateEntity("Entity", entity);
			s_SelectedEntity = newEntity;
		}	

		/*if (ImGui::MenuItem("Copy"))
		{

		}

		if (ImGui::MenuItem("Cut"))
		{

		}

		if (ImGui::MenuItem("Paste"))
		{

		}*/

		if (ImGui::MenuItem("Delete",nullptr, false, entity != nullptr))
		{
			if (s_SelectedEntity == entity)
				s_SelectedEntity = nullptr;
			m_scene->RemoveEntity(entity->GetUUID());
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Cube"))
			{
				std::shared_ptr<Entity> newEntity = m_scene->CreateEntity("Cube", entity);
				MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();

				std::string modelPath = "assets/models/primitives/cube.fbx";
				Metadata& meta = AssetRegistry::GetOrCreateMetadata(modelPath);
				MeshImporter::ImportModel(modelPath, meta);
				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meta.UUID, 0);
				if (mesh)
					renderer->SetMesh(mesh);

				s_SelectedEntity = newEntity;
			}

			if (ImGui::MenuItem("Sphere"))
			{
				std::shared_ptr<Entity> newEntity = m_scene->CreateEntity("Sphere", entity);
				MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();

				std::string modelPath = "assets/models/primitives/sphere.fbx";
				Metadata& meta = AssetRegistry::GetOrCreateMetadata(modelPath);
				MeshImporter::ImportModel(modelPath, meta);
				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meta.UUID, 0);
				if(mesh)
					renderer->SetMesh(mesh);

				s_SelectedEntity = newEntity;
			}

			if (ImGui::MenuItem("Cone"))
			{
				std::shared_ptr<Entity> newEntity = m_scene->CreateEntity("Cone", entity);
				MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();

				std::string modelPath = "assets/models/primitives/cone.fbx";
				Metadata& meta = AssetRegistry::GetOrCreateMetadata(modelPath);
				MeshImporter::ImportModel(modelPath, meta);
				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meta.UUID, 0);
				if (mesh)
					renderer->SetMesh(mesh);

				s_SelectedEntity = newEntity;
			}

			if (ImGui::MenuItem("Cylinder"))
			{
				std::shared_ptr<Entity> newEntity = m_scene->CreateEntity("Cylinder", entity);
				MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();

				std::string modelPath = "assets/models/primitives/cylinder.fbx";
				Metadata& meta = AssetRegistry::GetOrCreateMetadata(modelPath);
				MeshImporter::ImportModel(modelPath, meta);
				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meta.UUID, 0);
				if (mesh)
					renderer->SetMesh(mesh);

				s_SelectedEntity = newEntity;
			}

			ImGui::EndMenu();
		}
	}

	void HierarchyInterface::HotKeysSelectedEntiy()
	{
		if (!s_SelectedEntity) {
			return;
		}

		const InputEventManager& input = Application::GetInstance().GetInputEvent();

		if (input.GetKeyStatus(SDL_SCANCODE_DELETE) == KeyState::DOWN) {
			m_scene->RemoveEntity(s_SelectedEntity->GetUUID());
			s_SelectedEntity = nullptr;
		}

		if (input.GetKeyWithModifier(SDL_SCANCODE_C, KeyModifier::CTRL)) {
			/// Copy
		}

		if (input.GetKeyWithModifier(SDL_SCANCODE_V, KeyModifier::CTRL)) {
			/// Paste
		}

		if (input.GetKeyWithModifier(SDL_SCANCODE_X, KeyModifier::CTRL)) {
			/// Cut
		}

	}
}