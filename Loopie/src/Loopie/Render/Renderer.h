#pragma once
#include "Loopie/Core/Math.h"
#include "Loopie/Render/Shader.h"
#include "Loopie/Render/VertexArray.h"

namespace Loopie {
	class Renderer {
	public:
		static void Init(void* context);

		static void Clear();
		static void SetClearColor(vec4 color);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		static void Draw(const VertexArray& vao, const Shader& shader);
	};
}