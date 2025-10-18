#pragma once

#include <memory>
#include <vector>
#include <string>

namespace Loopie {
	class MeshImporter {
	public:
		static std::vector<std::string> LoadModel(const std::string& filepath);
		static bool CheckIfIsModel(const char* path);

	private:
		static void ProcessNode(void* node, const void* scene, std::vector<std::string>& outputPaths);
		static std::string ProcessMesh(void* mesh, const void* scene);
	};
}