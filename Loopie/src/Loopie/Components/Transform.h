#include "Loopie/Components/Component.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Core/Math.h"
#include <memory>
namespace Loopie
{
    class Transform : public Component
    {
    public:
        DEFINE_TYPE(Transform)
        Transform(vec3 pos = { 0,0,0 }, quaternion rot = {1,0,0,0}, vec3 sca = {1,1,1});
        ~Transform() = default;
        void Init()override;
#pragma region Transformations
        void Translate(const vec3& translation, bool local = true);
        void RotateAxisAngle(const vec3& axis, float angle, bool local = true);
        void RotateEulerAngles(const vec3& eulerAngles, bool local = true);
        void Scale(const vec3& scaling, bool local = true);
#pragma endregion
#pragma region Vectors
        vec3 Right();
        vec3 Up();
        vec3 Forward();  
#pragma endregion
#pragma region Matrix
        matrix4 GetMatrix();
        void UpdateMatrix();
#pragma endregion
#pragma region Get
        vec3 GetPosition() const;
        vec3 GetLocalPosition() const;
        quaternion GetRotation() const;
        quaternion GetLocalRotation() const;
        vec3 GetEulerAngles() const;
        vec3 GetLocalEulerAngles() const;
        vec3 GetScale() const;
        vec3 GetLocalScale() const;
        quaternion GetQuaternion() const;
#pragma endregion
#pragma region Set
        void SetPosition(const vec3& newPosition);
        void SetRotation(const vec3& newRotation);
        void SetQuaternion(const quaternion newQuaternion);
        void SetScale(const vec3& newScale);
#pragma endregion

        quaternion EulerAnglesToQuaternion(const vec3& eulerAngles);
        void UpdateParent();
    private:
        void Recalculate();
    private:
        vec3 m_position;
        vec3 m_localPosition;
        quaternion m_rotation;
        quaternion m_localRotation;
        vec3 m_eulerAngles;
        vec3 m_localEulerAngles;
        vec3 m_scale;
        vec3 m_localScale;
        matrix4 m_globalMatrix;

        Transform* m_parentTransform;
    };
}