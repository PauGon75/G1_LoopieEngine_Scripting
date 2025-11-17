#include "src/Interfaces/Interface.h"
#include "Loopie/Render/FrameBuffer.h"
#include "Loopie/Render/FrameBuffer.h"

namespace Loopie {
	class Camera;
	class GameInterface : public Interface {
	public:
		GameInterface();
		~GameInterface() = default;
		void Init() override {}
		void Render() override;

		void StartScene();
		void EndScene();

		Camera* GetCamera();

	private:
		std::shared_ptr<FrameBuffer> m_buffer;
		ImVec2 m_windowSize;
	};
}