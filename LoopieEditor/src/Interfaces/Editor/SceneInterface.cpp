#include "SceneInterface.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/REnder/Renderer.h"
#include "Loopie/Components/Transform.h"

#include <imgui.h>

namespace Loopie {
	SceneInterface::SceneInterface() {
		m_buffer = std::make_shared<FrameBuffer>(1, 1);
		m_camera = std::make_shared<OrbitalCamera>();
		m_camera->GetCamera()->GetTransform()->SetPosition({ 0,10,50.f });
	}

	void SceneInterface::Update(float dt, const InputEventManager& inputEvent)
	{
		if (!m_focused && !m_interacted)
			return;
		m_camera->ProcessEvent(inputEvent);
		m_camera->Update(dt);
	}

	void SceneInterface::Render() {

		if (ImGui::Begin("Scene")) {
			m_windowSize =  ImGui::GetContentRegionAvail();
			m_focused = ImGui::IsWindowHovered();
			m_interacted = ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right) || ImGui::IsMouseDown(ImGuiMouseButton_Middle);

			ImGui::Image((ImTextureID)m_buffer->GetTextureId(), m_windowSize, ImVec2(0,1), ImVec2(1,0));
		}
		ImGui::End();
	}

	void SceneInterface::StartScene()
	{
		m_buffer->Bind();

		ImVec2 textureSize = ImVec2((float)m_buffer->GetWidth(), (float)m_buffer->GetHeight());
		if (m_windowSize.x != textureSize.x || m_windowSize.y != textureSize.y) {
			Renderer::SetViewport(0, 0, m_windowSize.x, m_windowSize.y);
			m_camera->GetCamera()->SetViewport(0, 0, m_windowSize.x, m_windowSize.y);
			m_buffer->Resize(m_windowSize.x, m_windowSize.y);
		}

		m_buffer->Clear();	
	}

	void SceneInterface::EndScene()
	{
		m_buffer->Unbind();
	}
}