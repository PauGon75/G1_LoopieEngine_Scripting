#include "Renderer.h"

#include "Loopie/Core/Assert.h"

#include <glad/glad.h>

namespace Loopie {
	void Renderer::Init(void* context) {
		ASSERT(!gladLoadGLLoader((GLADloadproc)context), "Failed to Initialize GLAD!");

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::SetClearColor(vec4 color) {
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void Renderer::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}

	void Renderer::Draw(const VertexArray& vao, const Shader& shader) {
		vao.Bind();
		shader.Bind();
		//// Bind VAO
		glDrawElements(GL_TRIANGLES, vao.GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}