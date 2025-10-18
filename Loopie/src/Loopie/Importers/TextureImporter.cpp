#include "TextureImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include <IL/il.h>
#include <IL/ilu.h>

namespace Loopie {
	std::string TextureImporter::LoadImage(const std::string& filepath)
	{
		std::string outputPath;

		ILuint imageID;
		ilGenImages(1, &imageID);
		ilBindImage(imageID);

		// If we need to flip, just use the following code
		//if (flipVertically)
		//	ilEnable(IL_ORIGIN_SET);
		//else
		//	ilDisable(IL_ORIGIN_SET);

		if (ilLoadImage(filepath.c_str()))
		{
			// Convert to RGBA format
			if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
			{
				Log::Error("Failed to convert image {0}", filepath);
				ilDeleteImages(1, &imageID);
				return "";
			}

			int width = ilGetInteger(IL_IMAGE_WIDTH);
			int height = ilGetInteger(IL_IMAGE_HEIGHT);
			int channels = ilGetInteger(IL_IMAGE_CHANNELS);

			ILubyte* data = ilGetData();


			Project project = Application::GetInstance().m_activeProject;
			std::filesystem::path pathToWrite = project.GetChachePath();
			UUID id;
			pathToWrite /= id.Get() + ".texture";

			std::ofstream fs(pathToWrite, std::ios::out | std::ios::binary | std::ios::app);

			fs.write(reinterpret_cast<const char*>(&width), sizeof width);
			fs.write(reinterpret_cast<const char*>(&height), sizeof height);
			fs.write(reinterpret_cast<const char*>(&channels), sizeof channels);

			size_t imageSize = static_cast<size_t>(width) * height * channels;
			fs.write(reinterpret_cast<const char*>(data), imageSize);

			fs.close();

			ilDeleteImages(1, &imageID);
			return pathToWrite.string();
		}
		else
		{
			Log::Error("Failed to load image {0}", filepath);
			ilDeleteImages(1, &imageID);
		}

		return "";
	}

	bool TextureImporter::CheckIfIsImage(const char* path)
	{
		ILuint imageID;
		ilGenImages(1, &imageID);
		ilBindImage(imageID);
		bool success = ilLoadImage(path);
		ilDeleteImages(1, &imageID);

		return success;
	}
}