#pragma once
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Resources/MetadataRegistry.h"
#include "Loopie/Importers/ImportSettings.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Resources/ResourceManager.h"
#include "loopie/Core/Log.h"
#include <memory>
#include <vector>
#include <string>
#include <Loopie/Resources/ScriptResource.h>
#include <iostream>

namespace Loopie {

	class ScriptResource;

	struct TextureImportSettigns : public ImportSettings {

		enum class Format {
			RGB,
			RGBA
		};

		enum class WrapMode {
			Repeat,
			Clamp,
			Mirror,
			MirrorOnce
		};

		enum class FilterMode {
			Point,
			Bilinear,
			Trilinear
		};

		unsigned int TextureMaxSize;
		Format TextureFormat;
		WrapMode TextureWrapMode;
		FilterMode TextureFilterMode;
		bool GenerateMipmaps;
		bool ReadAndWrite;
	};

    class TextureImporter {
    public:
        static void ImportImage(const std::string& filepath, Metadata& metadata);
        static void LoadImage(const std::string& filepath, Texture& texture);
        static bool CheckIfIsImage(const char* path);
		static bool CompileManual(const std::string& sourcePath, const std::string& outputPath);
		static std::shared_ptr<ScriptResource> Import(const std::string& path);
		static bool Compile(std::shared_ptr<ScriptResource> script);
		static void Test();
    };
}