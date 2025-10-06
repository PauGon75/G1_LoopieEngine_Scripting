#include "VertexArray.h"
#include <glad/glad.h>
namespace Loopie
{
    static GLenum ConvertGLVariableTypeToGlType(GLVariableType type) {
        switch (type)
        {
        case Loopie::GLVariableType::INT:
            return GL_INT;
            break;
        case Loopie::GLVariableType::FLOAT:
            return GL_FLOAT;
            break;
        case Loopie::GLVariableType::BOOL:
            return GL_BOOL;
            break;
        default:
            return GL_NONE;
            break;
        }
    }

    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddBuffer(VertexBuffer& vbo, IndexBuffer& ebo)
    {
        Bind();

        ebo.Bind();
        m_ebo = &ebo;
        vbo.Bind();
        m_vbo = &vbo;

        const BufferLayout& layout = m_vbo->GetLayout();
        unsigned int index = 0;
        for (const auto& element : layout.GetElements())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, element.Count, ConvertGLVariableTypeToGlType(element.Type), GL_FALSE, layout.GetStride(), (const void*) element.Offset);
            index++;
        }
        
        Unbind();
        m_vbo->Unbind();
        m_ebo->Unbind();
    }
    const IndexBuffer& Loopie::VertexArray::GetIndexBuffer() const
    {
        return *m_ebo;
    }
}