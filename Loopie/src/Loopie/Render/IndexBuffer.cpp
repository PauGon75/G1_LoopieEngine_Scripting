#include "IndexBuffer.h"

#include <glad/glad.h>

namespace Loopie
{
    IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    {
        glGenBuffers(1, &m_rendererID);
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
        Unbind();
        m_count = count;
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_rendererID);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    unsigned int IndexBuffer::GetCount() const
    {
        return m_count;
    }
}