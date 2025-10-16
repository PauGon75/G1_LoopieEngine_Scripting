#pragma once
#include "Loopie/Components/Component.h"

#include "Loopie/Resources/Types/Mesh.h"
#include "Loopie/Render/Shader.h"
#include "Loopie/Render/Texture.h"


#include <memory>

namespace Loopie {
	class MeshRenderer : public Component{
	public:
		DEFINE_TYPE(MeshRenderer)

		MeshRenderer();
		~MeshRenderer() = default;

		void Render();
		Shader& GetShader() { return m_shader; }
		std::shared_ptr<Mesh> GetMesh() { return m_mesh; }
		void SetMesh(std::shared_ptr<Mesh>);
		

		void Init() override; //// From Component
	private:

		Shader m_shader = Shader("assets/shaders/CorrectShader.shader");
		Texture m_texture = Texture("assets/texture.png");
		std::shared_ptr<Mesh> m_mesh;
	};
}