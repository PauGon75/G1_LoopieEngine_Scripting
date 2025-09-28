#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"

#include <SDL3/SDL_init.h> // TEMP INCLUDE FOR POLLING EVENTS
#include <SDL3/SDL.h>// TEMP INCLUDE FOR POLLING EVENTS
#include <imgui.h>// TEMP INCLUDE FOR POLLING EVENTS


#include "Loopie/Files/FileDialog.h"

namespace Loopie {
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();
		Log::Info("Starting Application...");

		ASSERT(s_Instance != nullptr, "Application is already created");
		s_Instance = this;

		Log::Info("Application Started");

		// Window Creation
		m_window = new Window();
		Log::Info("Window created successfully.");

		m_imguiManager.Init();

	}

	Application::~Application()
	{
		Log::Info("Closing Application...");

		for (Module* module : m_modules)
		{
			module->OnUnload();
			delete(module);
		}
		m_modules.clear();

		//// Cleaning
		delete(m_window); 
		m_window = nullptr;

		Log::Info("Application Closed");
	}

	void Application::AddModule(Module* module)
	{
		m_modules.emplace_back(module);
		module->OnLoad();
		
	}

	void Application::RemoveModule(Module* module)
	{
		auto it = std::find(m_modules.begin(), m_modules.end(), module);
		if (it == m_modules.end())
		{
			if (module)
				Log::Warn("Module to remove not found -> {0}", module->GetName());
			else
				Log::Error("Module to remove is null");
			return;		
		}	
		module->OnUnload();
		m_modules.erase(it);
	}

	Application& Application::GetInstance()
	{
		return *s_Instance;
	}

	Window* Application::GetWindow() const
	{
		return m_window;
	}

	InputEventManager& Application::GetInputEvent()
	{
		return m_inputEvent;
	}

	void Application::Run()
	{
		while (m_running)
		{

			m_window->ClearWindow(); ///Test -> this should be moved to a RenderClass in the future
			
			m_imguiManager.StartFrame();

			m_inputEvent.Update();
			ProcessEvents();

			for (Module* module : m_modules) {
				if (module->IsActive()) {
					module->OnUpdate();
				}
			}

			ImGui::ShowDemoWindow();
			m_imguiManager.EndFrame();

			m_window->Update();		
		}
	}

	void Application::ProcessEvents()
	{
		if (m_inputEvent.HasEvent(SDL_EVENT_QUIT))
		{
			Close();
			return;
		}

		if (m_inputEvent.HasEvent(SDL_EVENT_KEY_DOWN)) {
			if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F1) == KeyState::DOWN)
			{
				m_window->GetSize().second, m_window->GetPosition().first, m_window->GetPosition().second, m_window->IsFullscreen();
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F2) == KeyState::DOWN)
			{
				m_window->SetWindowFullscreen(!m_window->IsFullscreen());
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F3) == KeyState::DOWN)
			{
				m_window->SetResizable(true);
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F4) == KeyState::DOWN)
			{
				m_window->SetResizable(false);
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F5) == KeyState::DOWN)
			{
				m_window->SetTitle("Loopie!");
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F6) == KeyState::DOWN)
			{
				m_window->SetPosition(10, 10);
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F7) == KeyState::DOWN)
			{
				m_window->SetWindowSize(1280, 720);
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F8) == KeyState::DOWN)
			{
				m_window->SetWindowSize(1280, 720, true);
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
			{
				Close();
			}
		}
	}

	void Application::Close()
	{
		m_running = false;
	}
}
