#include "InspectorInterface.h"
#include "src/Interfaces/Editor/HierarchyInterface.h"

#include "Loopie/Components/Transform.h"
#include "Loopie/Components/Camera.h"
#include "Loopie/Components/MeshRenderer.h"

#include <imgui.h>

namespace Loopie {
	InspectorInterface::InspectorInterface() {

	}

	void InspectorInterface::Render() {
		if (ImGui::Begin("Inspector")) {

			if (HierarchyInterface::s_SelectedEntity == nullptr) {
				ImGui::End();
				return;
			}
			std::shared_ptr<Entity> entitySelected = HierarchyInterface::s_SelectedEntity;
			DrawEntityConfig(entitySelected);

			std::vector<Component*> components = entitySelected->GetComponents();
			for (auto* component : components) {
				if (component->GetTypeID() == Transform::GetTypeIDStatic()) {
					DrawTransform(static_cast<Transform*>(component));
				}
				else if (component->GetTypeID() == Camera::GetTypeIDStatic()) {
					DrawCamera(static_cast<Camera*>(component));
				}
				else if (component->GetTypeID() == MeshRenderer::GetTypeIDStatic()) {
					DrawMeshRenderer(static_cast<MeshRenderer*>(component));
				}
			}
		}
		ImGui::End();
	}

	void InspectorInterface::DrawEntityConfig(const std::shared_ptr<Entity>& entity)
	{
		char nameBuffer[256];
		memset(nameBuffer, 0, sizeof(nameBuffer));
		strncpy_s(nameBuffer, entity->GetName().c_str(), sizeof(nameBuffer) - 1);

		bool isActive = entity->GetIsActive();
		if (ImGui::Checkbox("##", &isActive)) {
			entity->SetIsActive(isActive);
		}

		ImGui::SameLine();
		
		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
			entity->SetName(std::string(nameBuffer));
		}
		
		ImGui::Separator();
		
		ImGui::TextDisabled("UUID: %s", entity->GetUUID().Get().c_str());

		ImGui::Separator();
	}

	void InspectorInterface::DrawTransform(Transform* transform)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			vec3 position = transform->GetPosition();
			vec3 rotation = transform->GetRotationEulerDeg();
			vec3 scale = transform->GetScale();

			if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
				transform->SetPosition(position);
			}
			if (ImGui::DragFloat3("Rotation", &rotation.x, 0.5f)) {
				transform->SetRotationEuler(rotation);
			}
			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
				transform->SetScale(scale);
			}
		}
	}

	void InspectorInterface::DrawCamera(Camera* camera)
	{
		if (ImGui::CollapsingHeader("Camera")) {
			float fov = camera->GetFov();
			float nearPlane = camera->GetNearPlane();
			float farPlane = camera->GetFarPlane();

			if (ImGui::DragFloat("Fov", &fov, 1.0f, 1.0f, 179.0f))
				camera->SetFov(fov);

			if (ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.01f, farPlane - 0.01f))
				camera->SetNearPlane(nearPlane);

			if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane + 0.1f, 10000.0f))
				camera->SetFarPlane(farPlane);
		}
	}

	void InspectorInterface::DrawMeshRenderer(MeshRenderer* meshRenderer)
	{
		if (ImGui::CollapsingHeader("Mesh Renderer")) {
			auto mesh = meshRenderer->GetMesh();
			ImGui::Text("Mesh: %s", mesh ? "Assigned" : "None");
			ImGui::Separator();
			//ImGui::Text("Shader: %s", meshRenderer->GetShader().GetName().c_str()); ????
		}
	}
}