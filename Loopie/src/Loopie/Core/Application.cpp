#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Render/Shader.h" // TEMP INCLUDE FOR SHADER TESTING
#include "Loopie/Render/VertexArray.h" // TEMP INCLUDE FOR SHADER TESTING
#include "Loopie/Render/VertexBuffer.h" // TEMP INCLUDE FOR SHADER TESTING
#include "Loopie/Render/IndexBuffer.h" // TEMP INCLUDE FOR SHADER TESTING
#include "Loopie/Core/Math.h" // TEMP INCLUDE FOR SHADER TESTING

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
		////TESTING VARIABLES

		float cubeVertices[]
		{
			//  Position                Color
				-0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
				0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
				0.5f, 0.5f, -0.5f,      0.0f, 0.0f, 1.0f,
				-0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
				-0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,
				0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,
				-0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,
		};

		unsigned int cubeIndices[]
		{
			// Top face
			3, 2, 6,
			6, 7, 3,
			// Bottom face
			0, 1, 5,
			5, 4, 0,
			// Left face
			0, 4, 7,
			7, 3, 0,
			// Right face
			1, 5, 6,
			6, 2, 1,
			// Back face
			0, 1, 2,
			2, 3, 0,
			// Front face
			4, 5, 6,
			6, 7, 4,
		};



		Shader shader = Shader("../../../Loopie/src/Loopie/Render/CorrectShader.shader");
		VertexBuffer vb = VertexBuffer(cubeVertices,sizeof(cubeVertices));
		IndexBuffer ib = IndexBuffer(cubeIndices,36);


		vb.GetLayout().AddLayoutElement(GLVariableType::FLOAT, 3);
		vb.GetLayout().AddLayoutElement(GLVariableType::FLOAT, 3);


		VertexArray va;
		va.AddBuffer(vb, ib);


		glm::vec3 position(0.0f, 0.0f, -5.0f);
		glm::vec3 forward(0.0f, 0.0f, 1.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::mat4 viewMatrix = glm::lookAt(position             // Camera Position
			, position + forward   // Target Position
			, up);                 // Up Vector
		// Projection Matrix
		const float FOV = 45.0f;
		const float NEAR_PLANE = 0.1f;
		const float FAR_PLANE = 100.0f;

		SDL_Time prevTime;
		SDL_GetCurrentTime(&prevTime);
		float rotation = 0.0f;
		const float SPEED = 100.0f;


		vec2 windowSize = m_window->GetSize();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(FOV), static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), NEAR_PLANE, FAR_PLANE);

		////

		while (m_running)
		{

			Renderer::Clear();
			
			m_imguiManager.StartFrame();

			m_inputEvent.Update();
			ProcessEvents();

			for (Module* module : m_modules) {
				if (module->IsActive()) {
					module->OnUpdate();
				}
			}

			if(m_renderInterface){
				for (Module* module : m_modules) {
					if (module->IsActive()) {
						module->OnInterfaceRender();
					}
				}
			}

			///// TEST AREA

			if (m_inputEvent.HasEvent(SDL_EVENT_WINDOW_RESIZED)) {
				windowSize = m_window->GetSize();
				ivec2 windowPosition = m_window->GetPosition();
				projectionMatrix = glm::perspective(glm::radians(FOV), static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), NEAR_PLANE, FAR_PLANE);
				glViewport(0,0, windowSize.x, windowSize.y);
			}

			glm::mat4 modelMatrix(1.0f);
			modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 modelViewProj = projectionMatrix * viewMatrix * modelMatrix;

			SDL_Time currentTime;
			SDL_GetCurrentTime(&currentTime);

			const float dt = (currentTime - prevTime) / 1000000000.0f;
			rotation += SPEED * dt;
			prevTime = currentTime;

			shader.Bind();
			shader.SetUniformMat4("modelViewProj", modelViewProj);
			Renderer::Draw(va, shader);

			/////


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
			// TEST - F7 FOR CORRECT SHADER TESTING
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F7) == KeyState::DOWN)
			{
				Shader* shader = new Shader("../../../Loopie/src/Loopie/Render/CorrectShader.shader");
				if (!shader->GetIsValidShader())
				{
					delete shader;
					shader = nullptr;
				}
				if (shader)
				{
					/*shader->PrintParsedVariables();*/
				}
			}
			// TEST - F8 FOR INCORRECT SHADER PATH
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F8) == KeyState::DOWN)
			{
				Shader* shader = new Shader("../../../Loopie/src/Loopie/Render/CorrectShaader.shader");
				if (!shader->GetIsValidShader())
				{
					delete shader;
					shader = nullptr;
				}
				if (shader)
				{
					/*shader->PrintParsedVariables();*/
				}
			}
			// TEST - F9 FOR FAILING SHADER TESTING
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_F9) == KeyState::DOWN)
			{
				Shader* shader = new Shader("../../../Loopie/src/Loopie/Render/WrongShader.shader");
				if (!shader->GetIsValidShader())
				{
					delete shader;
					shader = nullptr;
				}
				if (shader)
				{
					/*shader->PrintParsedVariables();*/
				}
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
			{
				Close();
			}
			else if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_I) == KeyState::DOWN){
				m_renderInterface = !m_renderInterface;
			}
		}
	}

	void Application::Close()
	{
		m_running = false;
	}
}
