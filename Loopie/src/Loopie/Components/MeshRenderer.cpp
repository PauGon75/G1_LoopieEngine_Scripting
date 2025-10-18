#include "MeshRenderer.h"

#include "Loopie/Render/Renderer.h"

namespace Loopie {

	MeshRenderer::MeshRenderer() {
		
	}

	void MeshRenderer::Render() {
		if(m_mesh)
			Renderer::Draw(m_mesh->m_vao, m_shader);
	}

	void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
	{
		m_mesh = mesh;
	}

	void MeshRenderer::SetTexture(std::shared_ptr<Texture> texture)
	{
		m_texture = texture;

		m_shader.Bind();
		m_texture->m_tb->Bind();
		m_shader.SetUniformInt("u_Texture", 0);
		m_shader.Unbind();
	}

	void MeshRenderer::Init()
	{
	}
}