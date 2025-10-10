#include "Transform.h"

namespace Loopie
{
	Transform::Transform(const vec3& position, const quaternion& rotation, const vec3& scale) : m_position(position), m_rotation(rotation), m_scale(scale){}

	matrix4 Transform::GetTransformMatrix() const
	{ return glm::translate(matrix4(1.0f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(matrix4(1.0f), m_scale); }

	vec3 Transform::GetEulerAnglesRad() const
	{ return glm::eulerAngles(m_rotation); }

	vec3 Transform::GetEulerAnglesDeg() const
	{ return glm::degrees(GetEulerAnglesRad()); }
	
	void Transform::SetEulerAnglesRad(const vec3& rotation)
	{
		m_rotation = glm::quat(rotation);
	}

	void Transform::SetEulerAnglesDeg(const vec3& rotation)
	{
		SetEulerAnglesRad(glm::radians(rotation));
	}
}