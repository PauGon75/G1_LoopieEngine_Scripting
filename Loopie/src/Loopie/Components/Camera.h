#pragma once
#include "Loopie/Core/Math.h"
#include "Loopie/Components/Transform.h"
namespace Loopie
{
	class Camera
	{
	public:
		Camera(Transform& transform, float fov = 45.0f, float near_plane = 0.1f, float far_plane = 200.0f);
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

	private:
		void CalculateMatrices() const;
	public:
		Transform& m_transform;

	private:
		vec4 m_viewport;

		float m_fov;
		float m_nearPlane;
		float m_farPlane;

		mutable matrix4 m_viewMatrix;
		mutable matrix4 m_projectionMatrix;
		mutable matrix4 m_viewProjectionMatrix;

		mutable bool m_dirty = true;
	};
}