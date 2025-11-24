#pragma once
#include "Loopie/Resources/Types/Material.h"
#include "Loopie/Resources/MetadataRegistry.h"

#include <memory>
#include <vector>
#include <string>


namespace Loopie {
	class MaterialImporter {
	public:
		static void ImportMaterial(const std::string& filepath, Metadata& metadata);
		static void SaveMaterial(const std::string& filepath, Material& material, Metadata& metadata);
		static void LoadMaterial(const std::string& path, Material& material);
		static bool CheckIfIsMaterial(const char* path);
	private:
		template<typename T>
		static std::string GLMToString(const T& value)
        {
            std::string out;
            unsigned int columns = value.length();
            unsigned int rows = value[0].length();

            for (int c = 0; c < columns; c++)
            {
                for (int r = 0; r < rows; r++)
                    out += std::to_string(value[c][r]) + ",";
            }

            if (!out.empty())
                out.pop_back();

            return out;
        }
	};
}