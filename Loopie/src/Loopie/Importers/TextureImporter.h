#pragma once

#include <memory>
#include <vector>
#include <string>

namespace Loopie {
	class TextureImporter {
	public:
		static std::string LoadImage(const std::string& filepath);
		static bool CheckIfIsImage(const char* path);
	};
}