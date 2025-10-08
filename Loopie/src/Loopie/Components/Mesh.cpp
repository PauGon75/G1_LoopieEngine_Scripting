#include "Mesh.h"

#include "Loopie/Render/Renderer.h"

namespace Loopie {
	Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const VertexComponents& components)
		: m_vertices(vertices), m_indices(indices), m_components(components), m_vbo(m_vertices.data(), (unsigned int)m_vertices.size() * sizeof(float)), m_ebo(indices.data(), (unsigned int)indices.size())
	{
		BufferLayout& layout = m_vbo.GetLayout();
		

		if(m_components.Position)
			layout.AddLayoutElement(0,GLVariableType::FLOAT, 3, "_Pos");
		if (m_components.TexCoord)
			layout.AddLayoutElement(1,GLVariableType::FLOAT, 2, "_TexCoord");
		if (m_components.Normal)
			layout.AddLayoutElement(2,GLVariableType::FLOAT, 3, "_Normal");
		if (m_components.Tangent)
			layout.AddLayoutElement(3,GLVariableType::FLOAT, 3, "_Tangent");
		if (m_components.Color)
			layout.AddLayoutElement(4,GLVariableType::FLOAT, 3, "_Color");

		m_vao.AddBuffer(m_vbo, m_ebo);
	}

	void Mesh::Render() {
		Renderer::Draw(m_vao, m_shader);
	}
}
