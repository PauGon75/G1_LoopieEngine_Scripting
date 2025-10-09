#pragma once

#include "Loopie/Core/Math.h"
#include "Loopie/Render/Shader.h"
#include "Loopie/Render/IndexBuffer.h"
#include "Loopie/Render/VertexBuffer.h"
#include "Loopie/Render/VertexArray.h"
#include "Loopie/Components/Texture.h"

#include <vector>

namespace Loopie {

	struct VertexComponents {
		bool Position = true;
		bool TexCoord = false;
		bool Tangent = false;
		bool Color = false;
		bool Normal = false;
	};

	class Mesh {
	public :
		Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const VertexComponents& components);
		~Mesh() = default;

		void Render();

		Shader& GetShader() { return m_shader; }
	private:
		std::vector<float> m_vertices;
		std::vector<unsigned int> m_indices;
		VertexComponents m_components;

		VertexArray m_vao;
		VertexBuffer m_vbo;
		IndexBuffer m_ebo;

		Shader m_shader = Shader("assets/shaders/CorrectShader.shader");
		Texture m_texture = Texture("assets/texture.jpg");
	};
}