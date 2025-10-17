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
        void Init() override; //// From Component

#pragma region Transform Matrix
        matrix4 GetTransformMatrix() const;
#pragma endregion

#pragma region Position
        const vec3& GetPosition() const;

        void SetPosition(const vec3& position);

        void Translate(const vec3& translation, bool localSpace = true);
#pragma endregion

#pragma region Rotation
        quaternion QuaternionGetRotation() const;
        vec3 DegreesGetEulerAngles() const;
        vec3 RadiansGetEulerAngles() const;

        void Quaternion_SetRotation(const quaternion& rotation);
        void DegreesSetRotation(const vec3& degrees);
        void RadiansSetRotation(const vec3& radians);

        void Quaternion_Rotate(const vec3& eulerDegrees);
        void DegreesRotate(const vec3& eulerDegrees);
        void RadiansRotate(const vec3& eulerRadians);

        void DegreesRotateAroundAxis(const vec3& axis, float degrees);
        void RadiansRotateAroundAxis(const vec3& axis, float radians);
        
        void LookAt(const vec3& target, const vec3& up = { 0, 1, 0 });
        
        void DegreesRotateLocal(const vec3& degrees);
        void RadiansRotateLocal(const vec3& radians);
#pragma endregion

#pragma region Scale
        const vec3& GetScale() const;
        
        void SetScale(const vec3& scale);
#pragma endregion

#pragma region Vector
        const vec3& Right() const;
        const vec3& Up() const;
        const vec3& Forward() const;
#pragma endregion

        
        


        









        vec3 TransformPoint(const vec3& localPoint) const;

        vec3 InverseTransformPoint(const vec3& worldPoint) const;




    private:
        void SetDirty() const;
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