#include "Mesh.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Resources/AssetRegistry.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Loopie {
	Mesh::Mesh(const UUID& id) : Resource(id)
	{
		
	}
	void Mesh::LoadFromFile(const std::string path)
	{
		//// Get Data From ChacheFile
		std::filesystem::path filepath = path;
		if (!std::filesystem::exists(path))
			return;


		/// READ
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			Log::Warn("Error opening .mesh file -> {0}", path.c_str());
			return;
		}

		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if (size <= 0) {
			Log::Warn("Error reading .mesh file -> {0}", path.c_str());
			return;
		}

		MeshData data;

		file.read(reinterpret_cast<char*>(&data.VerticesAmount), sizeof data.VerticesAmount);
		file.read(reinterpret_cast<char*>(&data.VertexElements), sizeof data.VertexElements);
		file.read(reinterpret_cast<char*>(&data.IndicesAmount), sizeof data.IndicesAmount);

		file.read(reinterpret_cast<char*>(&data.HasPosition), sizeof data.HasPosition);
		file.read(reinterpret_cast<char*>(&data.HasNormal), sizeof data.HasNormal);
		file.read(reinterpret_cast<char*>(&data.HasTexCoord), sizeof data.HasTexCoord);
		file.read(reinterpret_cast<char*>(&data.HasTangent), sizeof data.HasTangent);
		file.read(reinterpret_cast<char*>(&data.HasColor), sizeof data.HasColor);

		data.Vertices.clear();
		data.Vertices = std::vector<float>(data.VerticesAmount* data.VertexElements);

		for (unsigned int i = 0; i < data.VerticesAmount * data.VertexElements; ++i) {
			file.read(reinterpret_cast<char*>(&data.Vertices[i]), sizeof data.Vertices[i]);
		}
		data.Indices.clear();
		data.Indices = std::vector<unsigned int>(data.IndicesAmount);
		for (unsigned int i = 0; i < data.IndicesAmount; ++i) {
			file.read(reinterpret_cast<char*>(&data.Indices[i]), sizeof data.Indices[i]);
		}
		file.close();
		///

		m_vbo = std::make_shared<VertexBuffer>(data.Vertices.data(), (unsigned int)(sizeof(float) * data.VerticesAmount * data.VertexElements));
		m_ebo = std::make_shared<IndexBuffer>(data.Indices.data(), data.IndicesAmount);
		m_vao = std::make_shared<VertexArray>();

		BufferLayout& layout = m_vbo->GetLayout();

		if (data.HasPosition)
			layout.AddLayoutElement(0, GLVariableType::FLOAT, 3, "a_Position");
		if (data.HasTexCoord)
			layout.AddLayoutElement(1, GLVariableType::FLOAT, 2, "a_TexCoord");
		if (data.HasNormal)
			layout.AddLayoutElement(2, GLVariableType::FLOAT, 3, "a_Normal");
		if (data.HasTangent)
			layout.AddLayoutElement(3, GLVariableType::FLOAT, 3, "a_Tangent");
		if (data.HasColor)
			layout.AddLayoutElement(4, GLVariableType::FLOAT, 4, "a_Color");

		m_vao->AddBuffer(m_vbo.get(), m_ebo.get());
	}

	void Mesh::Reload()
	{
		AssetMetadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata && metadata->isValid) {
			LoadFromFile(metadata->cachePath);
		}
	}
}
