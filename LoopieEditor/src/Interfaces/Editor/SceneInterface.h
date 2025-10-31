#pragma once 

#include "src/Interfaces/Interface.h"
#include "Loopie/Render/FrameBuffer.h"
#include "src/Others/OrbitalCamera.h"

namespace Loopie {
	class SceneInterface : public Interface {
	public:
		SceneInterface();
		~SceneInterface() = default;
		void Init() override {}
		void Update(float dt, const InputEventManager& inputEvent) override;
		void Render() override;

		void StartScene();
		void EndScene();

		Camera* GetCamera() { return m_camera->GetCamera(); }

	private:
		std::shared_ptr<FrameBuffer> m_buffer;
		std::shared_ptr<OrbitalCamera> m_camera;

		bool m_focused = false;
		bool m_interacted = false;
		ImVec2 m_windowSize;
	};
}