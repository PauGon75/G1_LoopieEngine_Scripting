#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Components/Mesh.h"
#include "Loopie/Files/MeshImporter.h"


#include "Loopie/Core/Math.h" // TEMP INCLUDE FOR SHADER TESTING
#include "Loopie/Components/Camera.h" //TEMP TEST FOR CAMERA TESTING
#include "Loopie/Components/Transform.h" //TEMP TEST FOR CAMERA TESTING

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

	Window& Application::GetWindow()
	{
		return *m_window;
	}

	InputEventManager& Application::GetInputEvent()
	{
		return m_inputEvent;
	}

	void Application::Run()
	{
		////TESTING VARIABLES
		Camera camera;
		camera.SetPosition(vec3(0, 0, -50.f));
		Transform meshT;
		std::vector<Mesh*> meshes;

		float rotation = 0.0f;
		const float SPEED = 100.0f;

		////

		while (m_running)
		{

			Renderer::Clear();

			m_window->StartFrame();
			m_imguiManager.StartFrame();

			m_inputEvent.Update();

			m_window->ProcessEvents(m_inputEvent);
			ProcessEvents(m_inputEvent);

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

			if (m_inputEvent.HasFileBeenDropped()) {
				const char* fileName = m_inputEvent.GetDroppedFile(0);
				if (MeshImporter::CheckIfIsModel(fileName)) {
					for (size_t i = 0; i < meshes.size(); i++)
					{
						delete meshes[i];
					}
					meshes.clear();

					meshes = MeshImporter::LoadModel(fileName);
				}
			}
			if (m_inputEvent.HasEvent(SDL_EVENT_WINDOW_RESIZED)) {
				ivec2 windowSize = m_window->GetSize();
				camera.SetViewport(0, 0, windowSize.x, windowSize.y);
			}

			if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_W) == KeyState::REPEAT)
				camera.SetPosition(vec3(0, 0, camera.GetPosition().z + 10 * m_window->GetDeltaTime()));

			if (m_inputEvent.GetKeyStatus(SDL_SCANCODE_S) == KeyState::REPEAT)
				camera.SetPosition(vec3(0, 0, camera.GetPosition().z - 10 * m_window->GetDeltaTime()));


			rotation += SPEED * m_window->GetDeltaTime();
			vec3 rot = meshT.GetEulerAnglesDeg();
			rot.y += rotation;

			meshT.SetEulerAnglesDeg(rot);

			glm::mat4 modelViewProj = camera.GetViewProjectionMatrix() * meshT.GetTransformMatrix();


			for (size_t i = 0; i < meshes.size(); i++)
			{
				meshes[i]->GetShader().Bind();
				meshes[i]->GetShader().SetUniformMat4("modelViewProj", modelViewProj);
				meshes[i]->Render();
			}
			
			/////


			m_imguiManager.EndFrame();

			m_window->Update();		
		}

		///// TEST AREA
		for (size_t i = 0; i < meshes.size(); i++)
		{
			delete meshes[i];
		}
		meshes.clear();
		/////
	}

	void Application::ProcessEvents(InputEventManager& eventController)
	{
		if (eventController.HasEvent(SDL_EVENT_QUIT))
		{
			Close();
			return;
		}
		if (eventController.HasEvent(SDL_EVENT_KEY_DOWN)) {
			if (eventController.GetKeyStatus(SDL_SCANCODE_F11) == KeyState::DOWN)
			{
				m_window->SetWindowFullscreen(!m_window->IsFullscreen());
			}
			else if (eventController.GetKeyStatus(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
			{
				Close();
			}
			else if (eventController.GetKeyStatus(SDL_SCANCODE_I) == KeyState::DOWN){
				SetInterfaceState(!m_renderInterface);
			}
		}
	}

	void Application::Close()
	{
		m_running = false;
	}
}
