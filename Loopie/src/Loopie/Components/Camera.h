#pragma once
#include "Loopie/Core/Math.h"
#include "Loopie/Components/Component.h"

namespace Loopie
{
	class Camera : public Component
	{
	public:
		DEFINE_TYPE(Camera)

		Camera(float fov = 45.0f, float near_plane = 0.1f, float far_plane = 200.0f);
		~Camera() = default;

		void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		const matrix4& GetViewMatrix()const;
		const matrix4& GetProjectionMatrix()const;
		const matrix4& GetViewProjectionMatrix()const;

		void SetFov(float fov);
		float GetFov() const;

		void SetNearPlane(float nearPlane);
		float GetNearPlane() const;

		void SetFarPlane(float farPlane);
		float GetFarPlane() const;

		void SetDirty() const;

		void Init() override; //// From Component
	private:
		void CalculateMatrices() const;
	private:
		vec4 m_viewport = vec4(0,0,1,1);

		float m_fov=45.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane=200.0f;

		mutable matrix4 m_viewMatrix = matrix4(1);
		mutable matrix4 m_projectionMatrix = matrix4(1);
		mutable matrix4 m_viewProjectionMatrix = matrix4(1);

		mutable bool m_dirty = true;
	};
}