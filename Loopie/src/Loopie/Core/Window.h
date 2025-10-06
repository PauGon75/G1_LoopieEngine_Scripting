#pragma once

#include <SDL3/SDL_video.h>
#include "Loopie/Core/Math.h"

#define WINDOW_DEFAULT_WIDTH 800
#define WINDOW_DEFAULT_HEIGHT 600

namespace Loopie {
	class Window {
	public:
		Window();
		~Window();

		bool Update();

		ivec2 GetSize() const;
		ivec2 GetPosition() const;
		bool HasFocus() const;
		bool IsFullscreen() const;

		// TODO? Set logic resolution? https://wiki.libsdl.org/SDL3/SDL_SetRenderLogicalPresentation
		void SetWindowSize(int width, int height, bool savePreviousWindowsSize = false);
		void SetWindowFullscreen(bool enable, const SDL_DisplayMode* mode = nullptr);
		void SetResizable(bool enable);/// Maybe Do a getter also?
		void SetTitle(const char* title);/// Maybe Do a getter also?
		void SetPosition(int x, int y);
		// missing borderless, vsync, multi-monitor?

		SDL_Window* GetSDLWindow() const { return m_window; }
		SDL_GLContext GetSDL_GL_Context() const { return m_glContext; }

	private:
		SDL_Window* m_window = nullptr;
		SDL_GLContext m_glContext = NULL;
		int m_windowPrevWidth = 0;
		int m_windowPrevHeight = 0;
	};
}

