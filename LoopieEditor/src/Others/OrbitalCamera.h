#pragma once
#include "Loopie/Components/Camera.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Core/Application.h"

namespace Loopie
{
    class OrbitalCamera
    {
    public:
        OrbitalCamera();
        ~OrbitalCamera();

        void ProcessEvent(const InputEventManager& inputEvent);
        void Update(float dt);

        Camera* GetCamera() { return m_camera; }

    private:
        float m_cameraMovementSpeed = 5.f;
        float m_cameraRotationSpeed = 0.005f;
        float m_cameraZoomSpeed = 0.5f;
        float m_speedMultiplier = 2.f;

        float m_yaw = 0;
        float m_pitch = 0;

        glm::vec3 m_inputDirection = glm::vec3(0);
        glm::vec3 m_panDirection = glm::vec3(0);
        glm::vec3 m_inputRotation = glm::vec3(0);
        float m_zoomInput = 0;

        glm::vec3 m_orbitOffset = glm::vec3(0, 0, 5);

        bool m_complexMovement = false;

        std::shared_ptr<Entity> m_entityToPivot;
        std::shared_ptr<Entity> m_entity;
        Camera* m_camera;
    };
}
