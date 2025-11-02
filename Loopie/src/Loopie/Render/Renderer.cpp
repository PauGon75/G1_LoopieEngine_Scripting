#include "Renderer.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Render/Gizmo.h"
#include <iostream>

#include <glad/glad.h>
#include <IL/il.h>
#include <IL/ilu.h>

namespace Loopie {

	void Renderer::Init(void* context) {
		ASSERT(!gladLoadGLLoader((GLADloadproc)context), "Failed to Initialize GLAD!");

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// DevIL Init
		ilInit();
		iluInit();

		// Gizmo Data Structure Init
		Gizmo::Init();
	}

	void Renderer::Shutdown() {
		Gizmo::Shutdown();
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

	void Renderer::BeginScene(const matrix4 viewProjectionMatrix)
	{
		Gizmo::BeginGizmo(viewProjectionMatrix);
	}

	void Renderer::EndScene()
	{
		Gizmo::EndGizmo();
	}

	void Renderer::Draw(std::shared_ptr<VertexArray> vao, std::shared_ptr<Material> material, const Transform* transform) {
		vao->Bind();
		material->Bind();
		material->GetShader().SetUniformMat4("lp_Transform", transform->GetWorldToLocalMatrix());
		glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer().GetCount(), GL_UNSIGNED_INT, nullptr);
		vao->Unbind();
	}
}
