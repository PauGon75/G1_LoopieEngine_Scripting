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

		struct MeshData
		{
			//const char* Name = "";
			unsigned int verticesAmount = 0;
			unsigned int vertexElements = 0;
			unsigned int indicesAmount = 0;
			VertexComponents components;
		};

		MeshData data;

		file.read(reinterpret_cast<char*>(&data.verticesAmount), sizeof data.verticesAmount);
		file.read(reinterpret_cast<char*>(&data.vertexElements), sizeof data.vertexElements);
		file.read(reinterpret_cast<char*>(&data.indicesAmount), sizeof data.indicesAmount);

		file.read(reinterpret_cast<char*>(&data.components.Position), sizeof data.components.Position);
		file.read(reinterpret_cast<char*>(&data.components.Normal), sizeof data.components.Normal);
		file.read(reinterpret_cast<char*>(&data.components.TexCoord), sizeof data.components.TexCoord);
		file.read(reinterpret_cast<char*>(&data.components.Tangent), sizeof data.components.Tangent);
		file.read(reinterpret_cast<char*>(&data.components.Color), sizeof data.components.Color);

		m_vertices.clear();
		m_vertices = std::vector<float>(data.verticesAmount* data.vertexElements);

		for (unsigned int i = 0; i < data.verticesAmount * data.vertexElements; ++i) {
			file.read(reinterpret_cast<char*>(&m_vertices[i]), sizeof m_vertices[i]);
		}
		m_indices.clear();
		m_indices = std::vector<unsigned int>(data.indicesAmount);
		for (unsigned int i = 0; i < data.indicesAmount; ++i) {
			file.read(reinterpret_cast<char*>(&m_indices[i]), sizeof m_indices[i]);
		}
		file.close();
		///

		m_components = data.components;

		m_vbo = std::make_shared<VertexBuffer>(m_vertices.data(), (unsigned int)(sizeof(float) * data.verticesAmount * data.vertexElements));
		m_ebo = std::make_shared<IndexBuffer>(m_indices.data(), data.indicesAmount);
		m_vao = std::make_shared<VertexArray>();

		BufferLayout& layout = m_vbo->GetLayout();

		if (m_components.Position)
			layout.AddLayoutElement(0, GLVariableType::FLOAT, 3, "_Pos");
		if (m_components.TexCoord)
			layout.AddLayoutElement(1, GLVariableType::FLOAT, 2, "_TexCoord");
		if (m_components.Normal)
			layout.AddLayoutElement(2, GLVariableType::FLOAT, 3, "_Normal");
		if (m_components.Tangent)
			layout.AddLayoutElement(3, GLVariableType::FLOAT, 3, "_Tangent");
		if (m_components.Color)
			layout.AddLayoutElement(4, GLVariableType::FLOAT, 3, "_Color");

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
