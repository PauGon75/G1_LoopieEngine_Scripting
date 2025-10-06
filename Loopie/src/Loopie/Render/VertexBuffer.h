#pragma once

#include <vector>

#include "Loopie/Render/BufferLayout.h"
namespace Loopie
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, unsigned int size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		BufferLayout& GetLayout() { return m_layout; }
		void SetLayout(const BufferLayout& layout) { m_layout = layout; }

	private:
		unsigned int m_vboID;
		BufferLayout m_layout;
	};
}