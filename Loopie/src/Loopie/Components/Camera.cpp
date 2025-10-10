#include "Camera.h"

namespace Loopie
{
	Camera::Camera(float fov, float near_plane, float far_plane): m_fov(fov), m_near_plane(near_plane), m_far_plane(far_plane)
	{

	}

	void Camera::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		m_viewport = vec4(x, y, width, height);
		CalculateMatrices();
	}

	const matrix4& Camera::GetViewMatrix() const
	{
		return m_viewMatrix;
	}

	const matrix4& Camera::GetProjectionMatrix() const
	{
		return m_projectionMatrix;
	}

	const matrix4& Camera::GetViewProjectionMatrix() const
	{
		return m_viewProjectionMatrix;
	}

	

	void Camera::SetPosition(const vec3& position)
	{
		m_position = position;
		CalculateMatrices();
	}

	void Camera::SetRotation(const vec3& rotation)
	{
		m_rotation = rotation;
		CalculateMatrices();
	}

	vec3 Camera::GetPosition() const
	{
		return m_position;
	}

	vec3 Camera::GetRotation() const
	{
		return m_rotation;
	}

	void Loopie::Camera::CalculateMatrices()
	{
		m_viewMatrix = glm::lookAt(m_position, m_position + vec3(0,0,1), vec3(0, 1, 0));
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_viewport.z / m_viewport.w, m_near_plane, m_far_plane);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}
}