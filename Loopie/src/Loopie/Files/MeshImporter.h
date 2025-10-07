#pragma once

#include "Loopie/Components/Mesh.h"

#include <string>

namespace Loopie {
	class MeshImporter {
	public:
		static std::vector<Mesh*> LoadModel(const std::string& filepath);

	private:
		static void ProcessNode(void* node, const void* scene, std::vector<Mesh*>& meshes);
		static Mesh* ProcessMesh(void* mesh, const void* scene);
	};
}