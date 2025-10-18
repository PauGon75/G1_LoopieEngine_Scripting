#include "Texture.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Resources/AssetRegistry.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Loopie {
	Texture::Texture(const UUID& id) : Resource(id) {

	}

	void Texture::LoadFromFile(const std::string path)
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

		file.read(reinterpret_cast<char*>(&m_width), sizeof m_width);
		file.read(reinterpret_cast<char*>(&m_height), sizeof m_height);
		file.read(reinterpret_cast<char*>(&m_channels), sizeof m_channels);

		size_t imageSize = static_cast<size_t>(m_width) * m_height * m_channels;
		m_pixels.resize(imageSize);
		file.read(reinterpret_cast<char*>(m_pixels.data()), imageSize);

		file.close();

		m_tb = std::make_shared<TextureBuffer>(m_pixels.data(), m_width, m_height, m_channels);

	}

	void Texture::Reload()
	{
		AssetMetadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata && metadata->isValid) {
			LoadFromFile(metadata->cachePath);
		}
	}
}