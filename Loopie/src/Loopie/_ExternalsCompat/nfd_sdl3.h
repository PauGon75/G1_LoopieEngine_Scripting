#ifndef _NFD_SDL3_H
#define _NFD_SDL3_H

#include <SDL3/SDL.h>
#include <nfd.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#define NFD_INLINE inline
#else
#define NFD_INLINE static inline
#endif  // __cplusplus

/**
 * Converts an SDL3 window handle to a native window handle usable by NFDe.
 * @param sdlWindow The SDL_Window* (SDL3).
 * @param[out] nativeWindow The output native handle (populated only on success).
 * @return true on success, false on failure.
 *
 * If this returns false, you may simply pass a zero-initialized nfdwindowhandle_t
 * to NFDe (i.e. fallback to dialogs without parent window).
 */
NFD_INLINE bool NFD_GetNativeWindowFromSDL3Window(SDL_Window* sdlWindow,
    nfdwindowhandle_t* nativeWindow) {
    if (!sdlWindow || !nativeWindow) {
        SDL_Log("Invalid argument to NFD_GetNativeWindowFromSDL3Window");
        return false;
    }

    SDL_PropertiesID props = SDL_GetWindowProperties(sdlWindow);
    if (props == 0) {
        SDL_Log("SDL_GetWindowProperties failed: %s", SDL_GetError());
        return false;
    }

    // On Windows: query the HWND
    {
        void* hwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (hwnd) {
            nativeWindow->type = NFD_WINDOW_HANDLE_TYPE_WINDOWS;
            nativeWindow->handle = hwnd;
            return true;
        }
    }

    // On macOS (Cocoa), SDL defines SDL_PROP_WINDOW_COCOA_WINDOW_POINTER
#ifdef SDL_VIDEO_DRIVER_COCOA
    {
        void* nswindow = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        if (nswindow) {
            nativeWindow->type = NFD_WINDOW_HANDLE_TYPE_COCOA;
            nativeWindow->handle = nswindow;
            return true;
        }
    }
#endif

    // On X11
#ifdef SDL_VIDEO_DRIVER_X11
    {
        // In SDL3, X11 window might come via pointer or integer property (window XID)
        void* xwindow_ptr = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, NULL);
        if (xwindow_ptr) {
            nativeWindow->type = NFD_WINDOW_HANDLE_TYPE_X11;
            nativeWindow->handle = xwindow_ptr;
            return true;
        }
        // Also try integer property
        int64_t xwindow_num = 0;
        if (SDL_GetIntegerProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, &xwindow_num) == 0) {
            nativeWindow->type = NFD_WINDOW_HANDLE_TYPE_X11;
            nativeWindow->handle = (void*)(uintptr_t)xwindow_num;
            return true;
        }
    }
#endif

    // If we reach here, we didn't detect a supported backend
    SDL_Log("Unsupported or unknown native window type (SDL3)");
    return false;
}

#undef NFD_INLINE
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // _NFD_SDL3_H
