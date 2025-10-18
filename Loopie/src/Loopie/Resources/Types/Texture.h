#include "Loopie/Render/TextureBuffer.h"
#include "Loopie/Resources/Resource.h"

#include <vector>
#include <memory>

namespace Loopie {

	class Texture : public Resource {
		friend class MeshRenderer;
	public:
		DEFINE_TYPE(Texture)

		Texture(const UUID& id);
		~Texture() = default;

		void LoadFromFile(const std::string path) override;
		void Reload() override;
	private:
		std::vector<unsigned char> m_pixels;
		int m_width = 0;
		int m_height = 0;
		int m_channels = 0;

		std::shared_ptr<TextureBuffer> m_tb;
	};
}