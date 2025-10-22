#include "MeshImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Resources/Types/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <iostream>
#include <filesystem> // Used for checking the extension


namespace Loopie {
	std::vector<std::string> MeshImporter::LoadModel(const std::string& filepath) {
		std::vector<std::string> outputPaths;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || !scene->mRootNode) {
			Log::Error("Assimp Error: {0}", importer.GetErrorString());
			return outputPaths;
		}

		ProcessNode(scene->mRootNode, scene, outputPaths);
		return outputPaths;
	}

	bool MeshImporter::CheckIfIsModel(const char* path)
	{
		Assimp::Importer importer;
		std::string extension = std::filesystem::path(path).extension().string();

		for (char& c : extension)
		{
			c = std::tolower(static_cast<unsigned char>(c));
		}

		if (!extension.empty() && extension[0] == '.')
			extension = extension.substr(1);

		return importer.IsExtensionSupported(extension);
	}

	void MeshImporter::ProcessNode(void* nodePtr, const void* scenePtr, std::vector<std::string>& outputPaths) {
		auto node = static_cast<const aiNode*>(nodePtr);
		auto scene = static_cast<const aiScene*>(scenePtr);

		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			outputPaths.push_back(ProcessMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene, outputPaths);
		}
	}

	std::string MeshImporter::ProcessMesh(void* meshPtr, const void* scenePtr) {

		auto mesh = static_cast<const aiMesh*>(meshPtr);

		MeshData data;
		//data.Name = mesh->mName.C_Str();
		data.VerticesAmount = mesh->mNumVertices;

		data.HasPosition = mesh->mNumVertices > 0;
		data.HasTexCoord = mesh->mTextureCoords[0];
		data.HasNormal = mesh->HasNormals();
		data.HasTangent = mesh->HasTangentsAndBitangents();
		data.HasColor = mesh->HasVertexColors(0);

		data.VertexElements = data.HasPosition ? 3 : 0;
		data.VertexElements += data.HasTexCoord ? 2 : 0;
		data.VertexElements += data.HasNormal ? 3 : 0;
		data.VertexElements += data.HasTangent ? 3 : 0;
		data.VertexElements += data.HasColor ? 4 : 0;


		///// File Creation
		Project project = Application::GetInstance().m_activeProject;
		std::filesystem::path pathToWrite = project.GetChachePath();
		UUID id;
		pathToWrite /= id.Get() + ".mesh";

		std::ofstream fs(pathToWrite, std::ios::out | std::ios::binary | std::ios::app);

		//fs.write(data.Name, sizeof data.Name);
		fs.write(reinterpret_cast<const char*>(&data.VerticesAmount), sizeof data.VerticesAmount);
		fs.write(reinterpret_cast<const char*>(&data.VertexElements), sizeof data.VertexElements);


		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			data.IndicesAmount += face.mNumIndices;
		}

		fs.write(reinterpret_cast<const char*>(&data.IndicesAmount), sizeof data.IndicesAmount);

		fs.write(reinterpret_cast<const char*>(&data.HasPosition), sizeof data.HasPosition);
		fs.write(reinterpret_cast<const char*>(&data.HasNormal), sizeof data.HasNormal);
		fs.write(reinterpret_cast<const char*>(&data.HasTexCoord), sizeof data.HasTexCoord);
		fs.write(reinterpret_cast<const char*>(&data.HasTangent), sizeof data.HasTangent);
		fs.write(reinterpret_cast<const char*>(&data.HasColor), sizeof data.HasColor);


		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			///Position
			fs.write(reinterpret_cast<const char*>(&mesh->mVertices[i].x), sizeof mesh->mVertices[i].x);
			fs.write(reinterpret_cast<const char*>(&mesh->mVertices[i].y), sizeof mesh->mVertices[i].y);
			fs.write(reinterpret_cast<const char*>(&mesh->mVertices[i].z), sizeof mesh->mVertices[i].z);

			///TexCoords
			if (data.HasTexCoord) {
				fs.write(reinterpret_cast<const char*>(&mesh->mTextureCoords[0][i].x), sizeof mesh->mTextureCoords[0][i].x);
				fs.write(reinterpret_cast<const char*>(&mesh->mTextureCoords[0][i].y), sizeof mesh->mTextureCoords[0][i].y);
			}

			///Normals
			if (data.HasNormal) {

				fs.write(reinterpret_cast<const char*>(&mesh->mNormals[i].x), sizeof mesh->mNormals[i].x);
				fs.write(reinterpret_cast<const char*>(&mesh->mNormals[i].y), sizeof mesh->mNormals[i].y);
				fs.write(reinterpret_cast<const char*>(&mesh->mNormals[i].z), sizeof mesh->mNormals[i].z);
			}

			///Tangent
			if (data.HasTangent) {

				fs.write(reinterpret_cast<const char*>(&mesh->mTangents[i].x), sizeof mesh->mTangents[i].x);
				fs.write(reinterpret_cast<const char*>(&mesh->mTangents[i].y), sizeof mesh->mTangents[i].y);
				fs.write(reinterpret_cast<const char*>(&mesh->mTangents[i].z), sizeof mesh->mTangents[i].z);
			}

			///Color
			if (data.HasColor) {
				const aiColor4D& c = mesh->mColors[0][i];

				fs.write(reinterpret_cast<const char*>(&c.r), sizeof c.r);
				fs.write(reinterpret_cast<const char*>(&c.g), sizeof c.g);
				fs.write(reinterpret_cast<const char*>(&c.b), sizeof c.b);
				fs.write(reinterpret_cast<const char*>(&c.a), sizeof c.a);
			}

		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				fs.write(reinterpret_cast<const char*>(&face.mIndices[j]), sizeof face.mIndices[j]);
			}
		}
		fs.close();


		return pathToWrite.string();
	}
}