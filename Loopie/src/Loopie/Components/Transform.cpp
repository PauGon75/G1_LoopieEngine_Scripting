#include "Transform.h"
#include "Loopie/Scene/Scene.h"
namespace Loopie
{
    Transform::Transform(vec3 pos, quaternion rot, vec3 sca) :
        m_globalMatrix(1.0f),
        m_position(0.0f), m_rotation(1, 0, 0, 0), m_scale(1.0f), m_eulerAngles(0, 0, 0),
        m_localPosition(1.0f), m_localScale(1.0f), m_localRotation(1, 0, 0, 0), m_localEulerAngles(0, 0, 0)
    {
        SetRotation(pos);
        SetQuaternion(rot);
        SetScale(sca);
        UpdateParent();
    }
    void Transform::Init(){}
#pragma region Transformations

    void Transform::Translate(const vec3& translation, bool local)
    {
        if (local) {
            m_localPosition += translation;
            m_position += m_localRotation * translation;
        }
        else {
            m_position += m_rotation * translation;
        }
        Recalculate();
    }

    void Transform::RotateAxisAngle(const vec3& axis, float angle, bool local)
    {
        quaternion rotationquat = glm::angleAxis(glm::radians(angle), axis);

        if (local) {
            m_localRotation = rotationquat;
            m_localEulerAngles = glm::eulerAngles(m_localRotation);
        }
        else {
            m_rotation = rotationquat;
            m_eulerAngles = glm::eulerAngles(m_rotation);
        }
        Recalculate();
    }

    void Transform::RotateEulerAngles(const vec3& m_eulerAngles, bool local)
    {
        quaternion rotationquat = glm::quat(glm::radians(m_eulerAngles));

        if (local) {
            m_localRotation = rotationquat;
            m_localEulerAngles = glm::eulerAngles(m_localRotation);
        }
        else {
            m_rotation = rotationquat;
            this->m_eulerAngles = glm::eulerAngles(m_rotation);
        }
        Recalculate();
    }

    void Transform::Scale(const vec3& scaling, bool local)
    {
        if (local) {
            m_localScale *= scaling;
        }
        else {
            m_scale *= scaling;
        }

        Recalculate();
    }
#pragma endregion
#pragma region Vector
    vec3 Transform::Forward()
    {
        UpdateMatrix();
        return glm::normalize(m_globalMatrix[2]);
    }

    vec3 Transform::Up()
    {
        UpdateMatrix();
        return glm::normalize(m_globalMatrix[1]);
    }

    vec3 Transform::Right()
    {
        UpdateMatrix();
        return glm::normalize(m_globalMatrix[0]);
    }
#pragma endregion
#pragma region Matrix
    matrix4 Transform::GetMatrix()
    {
        UpdateMatrix();
        return m_globalMatrix;
    }
    void Transform::UpdateMatrix()
    {
        m_globalMatrix = matrix4(1.0f);
        m_globalMatrix = glm::translate(m_globalMatrix, m_localPosition);
        m_globalMatrix = glm::translate(m_globalMatrix, m_position);
        m_globalMatrix *= glm::mat4_cast(m_rotation * m_localRotation);
        m_globalMatrix = glm::scale(m_globalMatrix, m_localScale);
        m_globalMatrix = glm::scale(m_globalMatrix, m_scale);
    }
#pragma endregion
#pragma region Get
    vec3 Transform::GetPosition() const
    {
        return m_position;
    }
    vec3 Transform::GetLocalPosition() const
    {
        return m_localPosition;
    }
    quaternion Transform::GetRotation() const
    {
        return m_rotation;
    }
    quaternion Transform::GetLocalRotation() const
    {
        return m_localRotation;
    }
    vec3 Transform::GetEulerAngles() const
    {
        vec3 m_eulerAngles = glm::eulerAngles(m_rotation);
        return m_eulerAngles;
    }
    vec3 Transform::GetLocalEulerAngles() const
    {
        vec3 m_eulerAngles = glm::eulerAngles(m_localRotation);
        return m_eulerAngles;
    }
    vec3 Transform::GetScale() const
    {
        return m_scale;
    }
    vec3 Transform::GetLocalScale() const
    {
        return m_localScale;
    }
    quaternion Transform::GetQuaternion()const
    {
        return m_rotation;
    }
#pragma endregion
#pragma region Set
    void Transform::SetPosition(const vec3& newPosition)
    {
        m_position = newPosition;
    }
    void Transform::SetRotation(const vec3& newRotation)
    {
        m_eulerAngles = newRotation;
        m_rotation = EulerAnglesToQuaternion(m_eulerAngles);
    }
    void Transform::SetScale(const vec3& newScale)
    {
        m_scale = newScale;
    }
    void Transform::SetQuaternion(const quaternion newQuaternion)
    {
        m_rotation = newQuaternion;
    }
#pragma endregion
    void Transform::Recalculate()
    {
        UpdateMatrix();
        for (auto& child : GetOwner()->GetChildren())
        {
            child->GetTransform()->Recalculate();
        }
    }
    void Transform::UpdateParent()
    {
        Entity* parent = GetOwner()->GetParent();
        if (parent)
            m_parentTransform = parent->GetTransform();
        else
            m_parentTransform = nullptr;

    }
    quaternion Transform::EulerAnglesToQuaternion(const vec3& m_eulerAngles)
    {
        quaternion newRotation;
        newRotation = glm::angleAxis(m_eulerAngles.z, vec3(0, 0, 1));
        newRotation *= glm::angleAxis(m_eulerAngles.y, vec3(0, 1, 0));
        newRotation *= glm::angleAxis(m_eulerAngles.x, vec3(1, 0, 0));
        return newRotation;
    }
}