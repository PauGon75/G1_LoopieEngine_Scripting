#include "VertexArray.h"

#include <glad/glad.h>
namespace Loopie
{
    class VertexArray
    {
    private:
        unsigned int m_RendererID;

    public:
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

        void VertexArray::AddBuffer(const VertexBuffer& vbo, const IndexBuffer& ebo)
        {
            Bind();
            ebo.Bind();
            vbo.Bind();
            const BufferLayout& layout = vbo.GetLayout();
            unsigned int index = 0;
            for (const auto& element : layout.GetElements())
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, element.Count, /*Implementar funcion en el VertexArray Cpp que pase de GLVariableType a una de Opengl*/, GL_FALSE, layout.GetStride(), (const void*)element.Offset);
                index++;
            }
            Unbind();
        }
    };
}