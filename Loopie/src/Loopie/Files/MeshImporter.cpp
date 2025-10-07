#include "MeshImporter.h"

#include "Loopie/Core/Log.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Loopie {
	std::vector<Mesh*> MeshImporter::LoadModel(const std::string& filepath) {
		std::vector<Mesh*> meshes;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || !scene->mRootNode) {
			Log::Error("Assimp Error: {0}", importer.GetErrorString());
			return meshes;
		}

		ProcessNode(scene->mRootNode, scene, meshes);
		return meshes;
	}

	void MeshImporter::ProcessNode(void* nodePtr, const void* scenePtr, std::vector<Mesh*>& meshes) {
		auto node = static_cast<const aiNode*>(nodePtr);
		auto scene = static_cast<const aiScene*>(scenePtr);

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene, meshes);
		}
	}

	Mesh* MeshImporter::ProcessMesh(void* meshPtr, const void* scenePtr) {

		auto mesh = static_cast<const aiMesh*>(meshPtr);

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		vertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex{};
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			if (mesh->HasNormals()) {
				vertex.Normal = {
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
			}
			else {
				vertex.Normal = { 0.0f, 0.0f, 0.0f };
			}

			if (mesh->mTextureCoords[0]) {
				vertex.TexCoord = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}
			else {
				vertex.TexCoord = { 0.0f, 0.0f };
			}

			if (mesh->HasTangentsAndBitangents()) {

				vertex.Tangent = { 
					mesh->mTangents[i].x,
					mesh->mTangents[i].y,
					mesh->mTangents[i].z 
				};
			}
			else {
				vertex.Tangent = { 0.0f, 0.0f, 0.0f };
			}

			if (mesh->HasVertexColors(0)) {
				const aiColor4D& c = mesh->mColors[0][i];
				vertex.Color = { c.r, c.g, c.b };
			}
			else {
				vertex.Color = { 1.0f, 1.0f, 1.0f };
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		return new Mesh(vertices, indices);
	}
}