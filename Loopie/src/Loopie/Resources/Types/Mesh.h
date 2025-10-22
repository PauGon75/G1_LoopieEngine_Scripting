#pragma once
#include "Loopie/Resources/Resource.h"

#include "Loopie/Render/IndexBuffer.h"
#include "Loopie/Render/VertexBuffer.h"
#include "Loopie/Render/VertexArray.h"
#include <vector>
#include <memory>

namespace Loopie {

	struct MeshData {
		unsigned int VerticesAmount = 0;
		unsigned int VertexElements = 0;
		unsigned int IndicesAmount = 0;

		bool HasPosition = true;
		bool HasTexCoord = false;
		bool HasNormal = false;
		bool HasTangent = false;
		bool HasColor = false;

		std::vector<float> Vertices;
		std::vector<unsigned int> Indices;
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
		MeshData m_data;

		std::shared_ptr<VertexArray> m_vao;
		std::shared_ptr<VertexBuffer> m_vbo;
		std::shared_ptr<IndexBuffer> m_ebo;

	};
}