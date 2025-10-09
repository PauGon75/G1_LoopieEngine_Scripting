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

		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		VertexComponents components;

		components.Position = mesh->mNumVertices > 0;
		components.Normal = mesh->HasNormals();
		components.TexCoord = mesh->mTextureCoords[0];
		components.Tangent = mesh->HasTangentsAndBitangents();
		components.Color = mesh->HasVertexColors(0);

		int totalComponents = components.Position ? 3 : 0;
		totalComponents += components.Normal ? 3 : 0;
		totalComponents += components.TexCoord ? 2 : 0;
		totalComponents += components.Tangent ? 3 : 0;
		totalComponents += components.Color ? 3 : 0;

		vertices.reserve(totalComponents * mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			///Position
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);

			///TexCoords
			if (components.TexCoord) {
				vertices.push_back(mesh->mTextureCoords[0][i].x);
				vertices.push_back(mesh->mTextureCoords[0][i].y);
			}

			///Normals
			if (components.Normal) {
				vertices.push_back(mesh->mNormals[i].x);
				vertices.push_back(mesh->mNormals[i].y);
				vertices.push_back(mesh->mNormals[i].z);
			}

			///Tangent
			if (components.Tangent) {
				vertices.push_back(mesh->mTangents[i].x);
				vertices.push_back(mesh->mTangents[i].y);
				vertices.push_back(mesh->mTangents[i].z);
			}

			///Color
			if (components.Color) {
				const aiColor4D& c = mesh->mColors[0][i];
				vertices.push_back(c.r);
				vertices.push_back(c.g);
				vertices.push_back(c.b);
			}

		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		return new Mesh(vertices, indices, components);
	}
}