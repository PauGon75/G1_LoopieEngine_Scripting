#include "InputEventManager.h"
#include "Loopie/Core/Log.h"

#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#include <cmath>

namespace Loopie {
	InputEventManager::InputEventManager()
	{
		m_keyboard.fill(KeyState::IDLE);
		m_gamepad.fill(KeyState::IDLE);
		m_mouse.fill(KeyState::IDLE);
		m_axes.fill(0.0f);

		m_touchedEvents.reserve(15);

	}
	InputEventManager::~InputEventManager()
	{
		if (gamepad) {
			SDL_CloseGamepad(gamepad);
			gamepad = nullptr;
		}
	}
	void InputEventManager::Update() {

		for (Uint32 t : m_touchedEvents) {
			m_events[t] = false;
		}
		m_touchedEvents.clear();

		AdvanceKeyStates(m_keyboard);
		AdvanceKeyStates(m_gamepad);
		AdvanceKeyStates(m_mouse);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);

			if (!m_events[event.type]) {
				m_events.set(event.type, true);
				m_touchedEvents.push_back(event.type);
			}

			switch (event.type) {
				case SDL_EVENT_KEY_DOWN:
					if (!event.key.repeat)
						m_keyboard[event.key.scancode] = KeyState::DOWN;
					break;

				case SDL_EVENT_KEY_UP:
					m_keyboard[event.key.scancode] = KeyState::UP;
					break;

				case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
					m_gamepad[event.gbutton.button] = KeyState::DOWN;
					break;

				case SDL_EVENT_GAMEPAD_BUTTON_UP:
					m_gamepad[event.gbutton.button] = KeyState::UP;
					break;

				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					if (event.button.button <= m_mouse.size())
						m_mouse[event.button.button - 1] = KeyState::DOWN;
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					if (event.button.button <= m_mouse.size())
						m_mouse[event.button.button - 1] = KeyState::UP;
					break;

				case SDL_EVENT_MOUSE_MOTION:
					m_mousePosition = { event.motion.x, event.motion.y };
					break;

				case SDL_EVENT_GAMEPAD_AXIS_MOTION:
					if (event.gaxis.value > 0)
						m_axes[event.gaxis.axis] = (event.gaxis.value) / 32767.0f;
					else
						m_axes[event.gaxis.axis] = (event.gaxis.value) / 32768.0f;

					if (std::fabs(m_axes[event.gaxis.axis]) < m_axisDeadZone)
						m_axes[event.gaxis.axis] = 0.0f;
					break;

				case SDL_EVENT_GAMEPAD_ADDED: {
					SDL_Gamepad* gamepad = SDL_OpenGamepad(event.gdevice.which);
					if (gamepad) {
						Log::Info("Gamepad connected: {0}", SDL_GetGamepadName(gamepad));
					}
					break;
				}

				case SDL_EVENT_GAMEPAD_REMOVED: {
					Log::Info("Gamepad removed");
					if (gamepad) {
						SDL_CloseGamepad(gamepad);
						gamepad = nullptr;
					}
					break;
				}

				default:
					break;
			}

		}
	}

	KeyState InputEventManager::GetKeyStatus(SDL_Scancode keyCode) const
	{
		return m_keyboard[keyCode];
	}

	KeyState InputEventManager::GetGamepadButtonStatus(SDL_GamepadButton controlCode) const
	{		
		return m_gamepad[controlCode];
	}

	KeyState InputEventManager::GetMouseButtonStatus(int mouseIndex) const
	{
		return m_mouse[mouseIndex];
	}

	const vec2& InputEventManager::GetMousePosition() const
	{
		return m_mousePosition;
	}

	vec2 InputEventManager::GetLeftAxis() const
	{
		return { m_axes[SDL_GAMEPAD_AXIS_LEFTX], m_axes[SDL_GAMEPAD_AXIS_LEFTY] };
	}

	vec2 InputEventManager::GetRightAxis() const
	{
		return { m_axes[SDL_GAMEPAD_AXIS_RIGHTX], m_axes[SDL_GAMEPAD_AXIS_RIGHTY] };
	}

	float InputEventManager::GetLeftTrigger() const
	{
		return m_axes[SDL_GAMEPAD_AXIS_LEFT_TRIGGER];
	}

	float InputEventManager::GetRightTrigger() const
	{
		return m_axes[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER];
	}
}