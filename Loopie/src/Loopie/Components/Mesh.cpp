#include "Mesh.h"

#include "Loopie/Render/Renderer.h"

namespace Loopie {
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
		: m_vertices(vertices), m_indices(indices), m_vbo(m_vertices.data(), (unsigned int)m_vertices.size() * sizeof(Vertex)), m_ebo(indices.data(), (unsigned int)indices.size())
	{
		BufferLayout& layout = m_vbo.GetLayout();
		layout.AddLayoutElement(GLVariableType::FLOAT, 3);
		layout.AddLayoutElement(GLVariableType::FLOAT, 2);
		layout.AddLayoutElement(GLVariableType::FLOAT, 3);
		layout.AddLayoutElement(GLVariableType::FLOAT, 3);
		layout.AddLayoutElement(GLVariableType::FLOAT, 3);

		m_vao.AddBuffer(m_vbo, m_ebo);
	}

	void Mesh::Render() {
		Renderer::Draw(m_vao, m_shader);
	}
}
