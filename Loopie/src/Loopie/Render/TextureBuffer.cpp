#include "TextureBuffer.h"

#include "Loopie/Core/Log.h"

#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#include <glad/glad.h>

namespace Loopie
{
	TextureBuffer::TextureBuffer(const unsigned char* data, int width, int height, int channels) : m_texture_ID(0)
	{
		GLenum format = GL_RGB;
		if (channels == 1)
			format = GL_RED;
		else if (channels == 4)
			format = GL_RGBA;

		glGenTextures(1, &m_texture_ID);
		glBindTexture(GL_TEXTURE_2D, m_texture_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	TextureBuffer::~TextureBuffer()
	{
		glDeleteTextures(1, &m_texture_ID);
	}

	void TextureBuffer::Bind(unsigned int unit) const
	{
		if (unit > 31)
		{
			Log::Error("Texture unit {0} exceeds maximum (31).", unit);
			return;
		}
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_texture_ID);

	}

	void TextureBuffer::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}