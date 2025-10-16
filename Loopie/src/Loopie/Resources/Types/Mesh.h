#pragma once
#include "Loopie/Resources/Resource.h"

#include "Loopie/Render/IndexBuffer.h"
#include "Loopie/Render/VertexBuffer.h"
#include "Loopie/Render/VertexArray.h"
#include <vector>
#include <memory>

namespace Loopie {

	struct VertexComponents {
		bool Position = true;
		bool TexCoord = false;
		bool Tangent = false;
		bool Color = false;
		bool Normal = false;
	};

	class Mesh : public Resource{
		friend class MeshRenderer;
	public :
		DEFINE_TYPE(Mesh)

		Mesh(const UUID& id);
		~Mesh() = default;

		void LoadFromFile(const std::string path) override;
		void Reload() override;
	private:
		std::vector<float> m_vertices;
		std::vector<unsigned int> m_indices;
		VertexComponents m_components;

		std::shared_ptr<VertexArray> m_vao;
		std::shared_ptr<VertexBuffer> m_vbo;
		std::shared_ptr<IndexBuffer> m_ebo;

	};
}