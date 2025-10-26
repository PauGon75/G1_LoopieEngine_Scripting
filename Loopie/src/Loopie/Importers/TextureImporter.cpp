#include "TextureImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include <IL/il.h>
#include <IL/ilu.h>

namespace Loopie {
	std::string TextureImporter::ImportImage(const std::string& filepath)
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
			pathToWrite /= "Textures";
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

	void TextureImporter::LoadImage(const std::string& path, Texture& texture)
	{
		std::filesystem::path filepath = path;
		if (!std::filesystem::exists(path))
			return;


		/// READ
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			Log::Warn("Error opening .texture file -> {0}", path.c_str());
			return;
		}

		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if (size <= 0) {
			Log::Warn("Error reading .texture file -> {0}", path.c_str());
			return;
		}

		file.read(reinterpret_cast<char*>(&texture.m_width), sizeof texture.m_width);
		file.read(reinterpret_cast<char*>(&texture.m_height), sizeof texture.m_height);
		file.read(reinterpret_cast<char*>(&texture.m_channels), sizeof texture.m_channels);

		size_t imageSize = static_cast<size_t>(texture.m_width) * texture.m_height * texture.m_channels;
		texture.m_pixels.resize(imageSize);
		file.read(reinterpret_cast<char*>(texture.m_pixels.data()), imageSize);

		file.close();

		texture.m_tb = std::make_shared<TextureBuffer>(texture.m_pixels.data(), texture.m_width, texture.m_height, texture.m_channels);
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