#include "Texture.h"
#include "stb_image.h"
#include "Loopie/Core/Log.h"
#include <iostream>
#include <glad/glad.h>
namespace Loopie
{
	Texture::Texture(const std::string& path, bool flipVertically) : m_Texture_ID(0), m_Width(0), m_Height(0), m_Channels(0)
	{
		stbi_set_flip_vertically_on_load(flipVertically);
		unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 4);
		if (data)
		{
			GLenum format = GL_RGB;
			if (m_Channels == 1)
				format = GL_RED;
			else if (m_Channels == 4)
				format = GL_RGBA;

			glGenTextures(1, &m_Texture_ID);
			glBindTexture(GL_TEXTURE_2D, m_Texture_ID);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
			if(data)
				stbi_image_free(data);
		}
		else
		{
			Log::Error("Failed to load image {0}", path);
			stbi_image_free(data);
		}
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_Texture_ID);
	}

	void Texture::Bind(unsigned int unit) const
	{
		if (unit > 31)
		{
			Log::Error("Texture unit {0} exceeds maximum (31).", unit);
			return;
		}
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_Texture_ID);

	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}