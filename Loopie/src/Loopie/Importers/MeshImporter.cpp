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

		struct MeshData
		{
			//const char* Name="";
			unsigned int verticesAmount=0;
			unsigned int vertexElements=0;
			unsigned int indicesAmount=0;
			VertexComponents components;
		};

		MeshData data;
		//data.Name = mesh->mName.C_Str();
		data.verticesAmount = mesh->mNumVertices;

		data.components.Position = mesh->mNumVertices > 0;
		data.components.Normal = mesh->HasNormals();
		data.components.TexCoord = mesh->mTextureCoords[0];
		data.components.Tangent = mesh->HasTangentsAndBitangents();
		data.components.Color = mesh->HasVertexColors(0);

		data.vertexElements = data.components.Position ? 3 : 0;
		data.vertexElements += data.components.Normal ? 3 : 0;
		data.vertexElements += data.components.TexCoord ? 2 : 0;
		data.vertexElements += data.components.Tangent ? 3 : 0;
		data.vertexElements += data.components.Color ? 3 : 0;



		///// File Creation
		Project project = Application::GetInstance().m_activeProject;
		std::filesystem::path pathToWrite = project.GetChachePath();
		UUID id;
		pathToWrite /= id.Get() + ".mesh";

		std::ofstream fs(pathToWrite, std::ios::out | std::ios::binary | std::ios::app);

		//fs.write(data.Name, sizeof data.Name);
		fs.write(reinterpret_cast<const char*>(&data.verticesAmount), sizeof data.verticesAmount);
		fs.write(reinterpret_cast<const char*>(&data.vertexElements), sizeof data.vertexElements);


		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			data.indicesAmount += face.mNumIndices;
		}

		fs.write(reinterpret_cast<const char*>(&data.indicesAmount), sizeof data.indicesAmount);

		fs.write(reinterpret_cast<const char*>(&data.components.Position), sizeof data.components.Position);
		fs.write(reinterpret_cast<const char*>(&data.components.Normal), sizeof data.components.Normal);
		fs.write(reinterpret_cast<const char*>(&data.components.TexCoord), sizeof data.components.TexCoord);
		fs.write(reinterpret_cast<const char*>(&data.components.Tangent), sizeof data.components.Tangent);
		fs.write(reinterpret_cast<const char*>(&data.components.Color), sizeof data.components.Color);


		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			///Position
			fs.write(reinterpret_cast<const char*>(&mesh->mVertices[i].x), sizeof mesh->mVertices[i].x);
			fs.write(reinterpret_cast<const char*>(&mesh->mVertices[i].y), sizeof mesh->mVertices[i].y);
			fs.write(reinterpret_cast<const char*>(&mesh->mVertices[i].z), sizeof mesh->mVertices[i].z);

			///TexCoords
			if (data.components.TexCoord) {
				fs.write(reinterpret_cast<const char*>(&mesh->mTextureCoords[0][i].x), sizeof mesh->mTextureCoords[0][i].x);
				fs.write(reinterpret_cast<const char*>(&mesh->mTextureCoords[0][i].y), sizeof mesh->mTextureCoords[0][i].y);
			}

			///Normals
			if (data.components.Normal) {

				fs.write(reinterpret_cast<const char*>(&mesh->mNormals[i].x), sizeof mesh->mNormals[i].x);
				fs.write(reinterpret_cast<const char*>(&mesh->mNormals[i].y), sizeof mesh->mNormals[i].y);
				fs.write(reinterpret_cast<const char*>(&mesh->mNormals[i].z), sizeof mesh->mNormals[i].z);
			}

			///Tangent
			if (data.components.Tangent) {

				fs.write(reinterpret_cast<const char*>(&mesh->mTangents[i].x), sizeof mesh->mTangents[i].x);
				fs.write(reinterpret_cast<const char*>(&mesh->mTangents[i].y), sizeof mesh->mTangents[i].y);
				fs.write(reinterpret_cast<const char*>(&mesh->mTangents[i].z), sizeof mesh->mTangents[i].z);
			}

			///Color
			if (data.components.Color) {
				const aiColor4D& c = mesh->mColors[0][i];

				fs.write(reinterpret_cast<const char*>(&c.r), sizeof c.r);
				fs.write(reinterpret_cast<const char*>(&c.g), sizeof c.g);
				fs.write(reinterpret_cast<const char*>(&c.b), sizeof c.b);
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