#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Shader.h" // TEMP INCLUDE FOR SHADER TESTING

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
		delete module;
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
			for (Module* module : m_modules) {
				if (module->IsActive()) {
					module->OnInterfaceRender();
				}
			}
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
				m_window->GetSize().y, m_window->GetPosition().x, m_window->GetPosition().y, m_window->IsFullscreen();
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
			// TEST - F8 FOR CORRECT SHADER TESTING
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F8) == KeyState::DOWN)
			{
				/*m_window->SetWindowSize(1280, 720, true);*/

				const char* vertexShaderSource = "\n"
					"#version 460 core\n"
					"layout (location = 0) in vec3 inPos;\n"
					"layout (location = 1) in vec3 inColor;\n"
					"out vec3 vertexColor;\n"
					"uniform mat4 modelViewProj;\n"
					"void main()\n"
					"{\n"
					"gl_Position = modelViewProj * vec4(inPos, 1.0);\n"
					"vertexColor = inColor;\n"
					"}\0";

				const char* fragmentShaderSource = "\n"
					"#version 460 core\n"
					"in vec3 vertexColor;\n"
					"out vec4 FragColor;\n"
					"void main()\n"
					"{\n"
					"   FragColor = vec4(vertexColor, 1.0f);\n"
					"}\0";

				Shader* shader = new Shader(vertexShaderSource, fragmentShaderSource);
				if (!shader->GetIsValidShader())
				{
					delete shader;
					shader = nullptr;
				}
				if (shader)
				{
					shader->PrintParsedVariables();
				}
			}
			// TEST - F9 FOR FAILING SHADER TESTING
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F9) == KeyState::DOWN)
			{
				/*m_window->SetWindowSize(1280, 720, true);*/

				const char* vertexShaderSource = "\n"
					"#2\n";

				const char* fragmentShaderSource = "\n"
					"#version 460 core\n"
					"in vec3 vertexColor;\n"
					"out vec4 FragColor;\n"
					"void main()\n"
					"{\n"
					"   FragColor = vec4(vertexColor, 1.0f);\n"
					"}\0";

				Shader* shader = new Shader(vertexShaderSource, fragmentShaderSource);
				if (!shader->GetIsValidShader())
				{
					delete shader;
					shader = nullptr;
				}
				if (shader)
				{
					shader->PrintParsedVariables();
				}
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
