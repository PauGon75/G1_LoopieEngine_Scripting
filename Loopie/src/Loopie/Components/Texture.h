#pragma once
#include <string>
namespace Loopie
{
	class Texture
	{
	public:
		Texture(const std::string& path, bool flipVertically = false);
		~Texture();

		void Bind(unsigned int unit = 0)const;
		void Unbind()const;

		unsigned int GetID()const { return m_Texture_ID; }
	private:
		unsigned int m_Texture_ID;
		int m_Width;
		int m_Height;
		int m_Channels;
	};
}