#include "OrbitalCamera.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Core/Log.h"

namespace Loopie
{
	OrbitalCamera::OrbitalCamera()
	{
		m_entity = std::make_shared<Entity>("OrbitalCamera");
		m_entity->AddComponent<Transform>();
		m_camera = m_entity->AddComponent<Camera>();
	}

	OrbitalCamera::~OrbitalCamera()
	{
	}
	void OrbitalCamera::ProcessEvent(InputEventManager& inputEvent)
	{
		if (inputEvent.GetMouseButtonStatus(1) == KeyState::REPEAT)
		{
			m_inputDirection = vec3(-inputEvent.GetMouseDelta().x, inputEvent.GetMouseDelta().y, 0);
		}
		else if (inputEvent.GetMouseButtonStatus(2) == KeyState::REPEAT)
		{
			m_inputRotation = vec3(-inputEvent.GetMouseDelta().x, -inputEvent.GetMouseDelta().y, 0);
			if (inputEvent.GetKeyStatus(SDL_SCANCODE_LSHIFT) == KeyState::REPEAT)
			{
				m_inputDirection = vec3(0);
				m_speedMultiplier = 2.0f;
			}
			else
				m_speedMultiplier = 1.0f;
		
			m_inputDirection = vec3(0);
			if (inputEvent.GetKeyStatus(SDL_SCANCODE_W) == KeyState::REPEAT && m_inputDirection.z > -1)
				m_inputDirection.z -= 1;
			if (inputEvent.GetKeyStatus(SDL_SCANCODE_A) == KeyState::REPEAT && m_inputDirection.x > -1)
				m_inputDirection.x -= 1;
			if(inputEvent.GetKeyStatus(SDL_SCANCODE_S) == KeyState::REPEAT && m_inputDirection.z < 1)
				m_inputDirection.z += 1;
			if(inputEvent.GetKeyStatus(SDL_SCANCODE_D) == KeyState::REPEAT && m_inputDirection.x < 1)
				m_inputDirection.x += 1;
			m_inputDirection *= 10;
		}
		else
		{
			m_inputDirection = vec3(0);
			m_inputRotation = vec3(0);
		}

		m_inputDirection.z -= inputEvent.GetScrollDelta().y * 25;
	}

	void OrbitalCamera::Update(float dt)
	{
		Transform* transform = m_entity->GetTransform();
		
		// Update pitch and yaw
		m_yaw += - m_inputRotation.x * dt; // Yaw around world up
		m_pitch += m_inputRotation.y * dt; // Pitch around local right

		// Build new rotation from yaw and pitch — no roll
		glm::quat yawRotation = glm::normalize(glm::angleAxis(m_yaw, vec3(0, 1, 0))); // global Y up
		glm::quat pitchRotation = glm::normalize(glm::angleAxis(m_pitch, vec3(1, 0, 0))); // local X

		transform->SetRotation(yawRotation * pitchRotation);

		if (glm::length(m_inputDirection) > 0.001f)
			transform->Translate(m_inputDirection * m_speedMultiplier * m_directionSpeed * dt);
	}

}