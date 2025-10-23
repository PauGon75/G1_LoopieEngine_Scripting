#include "Camera.h"

#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h"


namespace Loopie
{
	Camera::Camera(float fov, float near_plane, float far_plane): m_fov(fov), m_nearPlane(near_plane), m_farPlane(far_plane)
	{
		
	}

	void Camera::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		m_viewport = vec4(x, y, width, height);
		SetDirty();
	}

	const matrix4& Camera::GetViewMatrix() const
	{
		CalculateMatrices();
		return m_viewMatrix;
	}

	const matrix4& Camera::GetProjectionMatrix() const
	{
		CalculateMatrices();
		return m_projectionMatrix;
	}

	const matrix4& Camera::GetViewProjectionMatrix() const
	{
		CalculateMatrices();
		return m_viewProjectionMatrix;
	}

	void Camera::SetFov(float fov)
	{
		m_fov = fov;
		SetDirty();
	}

	float Camera::GetFov() const
	{
		return m_fov;
	}

	void Camera::SetNearPlane(float nearPlane)
	{
		m_nearPlane = nearPlane;
		SetDirty();
	}

	float Camera::GetNearPlane() const
	{
		return m_nearPlane;
	}

	void Camera::SetFarPlane(float farPlane)
	{
		m_farPlane = farPlane;
		SetDirty();
	}

	float Camera::GetFarPlane() const
	{
		return m_farPlane;
	}


	void Camera::CalculateMatrices() const
	{
		if (!m_dirty)
			return;
		
		m_viewMatrix = glm::inverse(GetTransform()->GetMatrix());
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_viewport.z / m_viewport.w, m_nearPlane, m_farPlane);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void Camera::SetDirty() const{
		m_dirty = true;
	}
	void Camera::Init()
	{
		CalculateMatrices();
	}
}