#pragma once
#include "VertexBuffer.h"
#include "Loopie/Render/IndexBuffer.h"
#include "Loopie/Render/BufferLayout.h"

namespace Loopie
{
    class VertexArray
    {
    private:
        unsigned int m_RendererID;

    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void AddBuffer(const VertexBuffer& vbo, const IndexBuffer& ebo);
    };
}