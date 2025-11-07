#pragma once
#include "Loopie/Core/Math.h"
#include "Loopie/Resources/Types/Material.h"
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Render/VertexArray.h"

#include <filesystem>

namespace Loopie {
	class Transform;

	class Renderer {
	public:
		static void Init(void* context);
		static void Shutdown();

		static void Clear();
		static void SetClearColor(vec4 color);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void BeginScene(const matrix4 viewProjectionMatrix);
		static void EndScene();

		static void Draw(std::shared_ptr<VertexArray> vao, std::shared_ptr<Material> material, const Transform* transform);

		static void SetDefaultTexture(std::shared_ptr<Texture> texture) { s_DefaultTexture = texture; }

	public:
		static std::shared_ptr<Texture> s_DefaultTexture;
	};
}