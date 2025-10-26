#pragma once
#include "Loopie/Resources/Types/Material.h"

#include <memory>
#include <vector>
#include <string>


namespace Loopie {
	class MaterialImporter {
	public:
		static std::string ImportMaterial(const std::string& filepath);
		static void LoadMaterial(const std::string& path, Material& material);
		static bool CheckIfIsMaterial(const char* path);
	};
}