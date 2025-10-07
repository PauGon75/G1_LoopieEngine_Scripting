#include "VertexBuffer.h"

#include <glad/glad.h>

namespace Loopie
{
	VertexBuffer::VertexBuffer(const void* data, unsigned int size)
	{
		glGenBuffers(1, &m_rendererID);
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		Unbind();
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}