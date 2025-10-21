#pragma once 

#include "src/Interfaces/Interface.h"
#include "Loopie/Core/Math.h"

namespace Loopie {
	class Transform;
	class Camera;
	class MeshRenderer;


	class InspectorInterface : public Interface {
	public:
		InspectorInterface();
		~InspectorInterface() = default;
		void Init() override {}
		void Render() override;

	private:
		void DrawEntityConfig(const std::shared_ptr<Entity>& entity);
		void DrawTransform(Transform* transform);
		void DrawCamera(Camera* camera);
		void DrawMeshRenderer(MeshRenderer* meshRenderer);


		void DrawFloatField(float& values, vec3& color);
		void DrawVectorField(vec2& values);
		void DrawVectorField(vec3& values);
		void DrawVectorField(vec4& values);

		void PushMultiItemsWidths(int components, float width);
	};
}