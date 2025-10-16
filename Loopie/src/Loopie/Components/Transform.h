#pragma once

#include "Loopie/Core/Math.h"
#include "Loopie/Components/Component.h"
#include <functional>

namespace Loopie
{
    class Transform : public Component
    {
    public:
        DEFINE_TYPE(Transform)

        Transform::Transform(const vec3& position = { 0, 0, 0 }, const quaternion& rotation = { 1, 0, 0, 0 }, const vec3& scale = { 1, 1, 1 });
        
        Transform::~Transform() = default;

        matrix4 GetTransformMatrix() const;

        quaternion GetRotation() const;

        vec3 GetRotationEulerDeg() const;

        vec3 GetRotationEulerRad() const;

        void SetRotation(const quaternion& rotation);

        void SetRotationEuler(const vec3& eulerDegrees);

        void SetRotationEulerRad(const vec3& eulerRadians);

        void Rotate(const vec3& eulerDegrees);

        void RotateRad(const vec3& eulerRadians);

        void RotateAroundAxis(const vec3& axis, float degrees);

        void RotateAroundAxisRad(const vec3& axis, float radians);

        void SetPosition(const vec3& position);

        const vec3& GetPosition() const;

        void SetScale(const vec3& scale);

        const vec3& GetScale() const;

        void Translate(const vec3& translation, bool localSpace = true);

        void LookAt(const vec3& target, const vec3& up = { 0, 1, 0 });

        const vec3& Forward() const;

        const vec3& Right() const;

        const vec3& Up() const;

        vec3 TransformPoint(const vec3& localPoint) const;

        vec3 InverseTransformPoint(const vec3& worldPoint) const;

        void SetDirty() const;

        void Init() override; //// From Component

    private:
        void RecalculateCache() const;
    public:
        std::function<void()>OnTransformDirty;
    private:
        vec3 m_position;
        quaternion m_rotation;
        vec3 m_scale;

        mutable bool m_dirty = true;
        mutable matrix4 m_matrix;
        mutable vec3 m_forward;
        mutable vec3 m_right;
        mutable vec3 m_up;
    };
}