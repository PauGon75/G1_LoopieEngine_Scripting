#include "Transform.h"

namespace Loopie
{
    Transform::Transform(const vec3& position, const quaternion& rotation, const vec3& scale)
        : m_position(position), m_rotation(rotation), m_scale(scale)
    {
        SetDirty();
    }

    matrix4 Transform::GetTransformMatrix() const
    {
        RecalculateCache();
        return m_matrix;
    }

    quaternion Transform::GetRotation() const
    {
        return m_rotation;
    }

    vec3 Transform::GetRotationEulerDeg() const
    {
        return glm::degrees(glm::eulerAngles(m_rotation));
    }

    vec3 Transform::GetRotationEulerRad() const
    {
        return glm::eulerAngles(m_rotation);
    }

    void Transform::SetRotation(const quaternion& rotation)
    {
        m_rotation = rotation;
        SetDirty();
    }

    void Transform::SetRotationEuler(const vec3& eulerDegrees)
    {
        m_rotation = glm::quat(glm::radians(eulerDegrees));
        SetDirty();
    }

    void Transform::SetRotationEulerRad(const vec3& eulerRadians)
    {
        m_rotation = glm::quat(eulerRadians);
        SetDirty();
    }

    void Transform::Rotate(const vec3& eulerDegrees)
    {
        RotateRad(glm::radians(eulerDegrees));
    }

    void Transform::RotateRad(const vec3& eulerRadians)
    {
        m_rotation = glm::normalize(glm::quat(eulerRadians) * m_rotation);
        SetDirty();
    }

    void Transform::RotateAroundAxis(const vec3& axis, float degrees)
    {
        RotateAroundAxisRad(axis, glm::radians(degrees));
    }

    void Transform::RotateAroundAxisRad(const vec3& axis, float radians)
    {
        m_rotation = glm::normalize(glm::angleAxis(radians, glm::normalize(axis)) * m_rotation);
        SetDirty();
    }

    void Transform::SetPosition(const vec3& position)
    {
        m_position = position;
        SetDirty();
    }

    const vec3& Transform::GetPosition() const
    {
        return m_position;
    }

    void Transform::SetScale(const vec3& scale)
    {
        m_scale = scale;
        SetDirty();
    }

    const vec3& Transform::GetScale() const
    {
        return m_scale;
    }

    void Transform::Translate(const vec3& translation, bool localSpace)
    {
        if (localSpace)
            m_position += m_rotation * translation;
        else
            m_position += translation;
        SetDirty();
    }

    void Transform::LookAt(const vec3& target, const vec3& up)
    {
        vec3 forward = glm::normalize(target - m_position);
        vec3 right = glm::normalize(glm::cross(up, forward));
        vec3 correctedUp = glm::cross(forward, right);

        glm::mat3 rotMat(right, correctedUp, forward);
        m_rotation = glm::quat_cast(rotMat);
        SetDirty();
    }

    const vec3& Transform::Forward() const
    {
        RecalculateCache();
        return m_forward;
    }

    const vec3& Transform::Right() const
    {
        RecalculateCache();
        return m_right;
    }

    const vec3& Transform::Up() const
    {
        RecalculateCache();
        return m_up;
    }

    vec3 Transform::TransformPoint(const vec3& localPoint) const
    {
        return GetTransformMatrix() * glm::vec4(localPoint, 1.0f);
    }

    vec3 Transform::InverseTransformPoint(const vec3& worldPoint) const
    {
        return glm::inverse(GetTransformMatrix()) * glm::vec4(worldPoint, 1.0f);
    }

    void Transform::RecalculateCache() const
    {
        if (!m_dirty)
            return;

        m_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);

        m_forward = glm::normalize(m_rotation * glm::vec3(0, 0, 1));
        m_right = glm::normalize(m_rotation * glm::vec3(1, 0, 0));
        m_up = glm::normalize(m_rotation * glm::vec3(0, 1, 0));

        m_dirty = false;
    }

    void Transform::SetDirty() const
    {
        m_dirty = true;
        if (OnTransformDirty)
            OnTransformDirty();
    }
}