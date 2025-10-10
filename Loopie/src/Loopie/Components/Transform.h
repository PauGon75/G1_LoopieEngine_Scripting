#pragma once
#include "Loopie/Core/Math.h"

namespace Loopie
{
	class Transform
	{
	public:
		Transform(const vec3& position = {0,0,0}, const quaternion& rotation = {0,0,0,1}, const vec3& scale = {1,1,1});
		~Transform() = default;
		matrix4 GetTransformMatrix()const;
		vec3 GetEulerAnglesRad()const;
		vec3 GetEulerAnglesDeg()const;
		void SetEulerAnglesRad(const vec3& rotation);
		void SetEulerAnglesDeg(const vec3& rotation);
	public:
		vec3 m_position;
		quaternion m_rotation;
		vec3 m_scale;
	};
}