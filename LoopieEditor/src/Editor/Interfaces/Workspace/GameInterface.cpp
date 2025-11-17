#include "GameInterface.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Render/Renderer.h"

#include <imgui.h>


namespace Loopie {
	GameInterface::GameInterface() {
		m_buffer = std::make_shared<FrameBuffer>(1, 1);
	}

	void GameInterface::Render() {
		
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;
		if (ImGui::Begin("Game", nullptr, flags)) {
			m_windowSize = ImGui::GetContentRegionAvail();
			ImGui::Image((ImTextureID)m_buffer->GetTextureId(), m_windowSize, ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
	}

	Camera* GameInterface::GetCamera()
	{
		return Camera::GetMainCamera();
	}

	void GameInterface::StartScene()
	{
		m_buffer->Bind();

		if (!Camera::GetMainCamera()) {
			m_buffer->Clear();
			return;
		}

		vec2 textureSize = vec2((float)m_buffer->GetWidth(), (float)m_buffer->GetHeight());
		vec4 viewportSize = Camera::GetMainCamera()->GetViewport();
		Renderer::SetViewport(0, 0, m_windowSize.x, m_windowSize.y);

		if (m_windowSize.x != textureSize.x || m_windowSize.y != textureSize.y)
			m_buffer->Resize(m_windowSize.x, m_windowSize.y);
		if(m_windowSize.x != viewportSize.z || m_windowSize.y != viewportSize.w)
			Camera::GetMainCamera()->SetViewport(0, 0, m_windowSize.x, m_windowSize.y);

		m_buffer->Clear();
	}

	void GameInterface::EndScene()
	{
		m_buffer->Unbind();
	}
}