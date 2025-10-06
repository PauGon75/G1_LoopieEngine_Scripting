#pragma once
#include "Loopie/Core/Math.h"

#include <SDL3/SDL_events.h>
#include <bitset>
#include <vector>
#include <array>

namespace Loopie {
	
	enum class KeyState {
		IDLE,
		UP,
		DOWN,
		REPEAT
	};

	class InputEventManager {
	public:
		InputEventManager();
		~InputEventManager();

		void Update();
		bool HasEvent(SDL_EventType eventType) const { return m_events[eventType]; }

		KeyState GetKeyStatus(SDL_Scancode keyCode) const;
		KeyState GetGamepadButtonStatus(SDL_GamepadButton controlCode) const;
		KeyState GetMouseButtonStatus(int mouseIndex) const;

		const vec2& GetMousePosition() const;
		vec2 GetLeftAxis() const;
		vec2 GetRightAxis() const;

		float GetLeftTrigger() const;
		float GetRightTrigger() const;

		void SetAxisDeadzone(float value) { m_axisDeadZone = value; }
		
	private:

		template <size_t N>
		void AdvanceKeyStates(std::array<KeyState, N>& arr) {
			for (auto& state : arr) {
				switch (state) {
					case KeyState::DOWN: 
						state = KeyState::REPEAT; 
						break;

					case KeyState::UP:   
						state = KeyState::IDLE;   
						break;

					default: 
						break;
				}
			}
		}
		
	private:
		std::bitset<SDL_EVENT_LAST> m_events;
		std::vector<Uint32> m_touchedEvents;


		std::array<KeyState, SDL_SCANCODE_COUNT> m_keyboard;
		std::array<KeyState, SDL_GAMEPAD_BUTTON_COUNT> m_gamepad;
		std::array<float, SDL_GAMEPAD_AXIS_COUNT> m_axes;
		std::array<KeyState, 5> m_mouse;

		vec2 m_mousePosition;

		float m_axisDeadZone = 0.15f;

		SDL_Gamepad* gamepad = nullptr;
	};
}