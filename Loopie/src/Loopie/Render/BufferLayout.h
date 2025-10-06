#pragma once

#include <vector>
#include <glad/glad.h>

namespace Loopie {

	enum class GLVariableType {
		INT,
		FLOAT,
		BOOL
	};

	static unsigned int GetGLVariableSize(GLVariableType type) {
		switch (type) {
			case GLVariableType::INT:
			case GLVariableType::FLOAT:
				return 4;
			case GLVariableType::BOOL:
				return 1;
			default:
				return 0;
		}
	}



	struct BufferElement {

		GLVariableType Type;
		unsigned int Count;
		unsigned int Offset;

		BufferElement(unsigned int Offset, const GLVariableType& Type, unsigned int Count)
			: Offset(Offset), Type(Type), Count(Count)
		{

		}
	};

	class BufferLayout {
	public:

		void AddLayoutElement(GLVariableType type, unsigned int count) {
			m_layout.emplace_back(BufferElement{ m_stride, type, count });
			m_stride+=GetGLVariableSize(type)*count;
		}

		unsigned int GetStride()const { return m_stride; }

		const std::vector <BufferElement>& GetElements()const { return m_layout; }

	private:
		std::vector<BufferElement> m_layout;
		unsigned int m_stride = 0;
	};
}