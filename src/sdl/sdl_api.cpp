#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <log/log.h>
#include "sdl_api.h"

namespace mkr {
    void sdl_api::init() {
        init_video();
        init_image();
        init_input();
        init_events();
    }

    void sdl_api::exit() {
        exit_events(); // Needs to exit first or else SDL_PollEvent will crash if the other subsystems are shutdown.
        exit_input();
        exit_image();
        exit_video();
    }

    void sdl_api::init_video() {
        if (0 != SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            const std::string err_msg = "SDL_INIT_VIDEO failed";
            MKR_CORE_ERROR(err_msg);
            throw std::runtime_error(err_msg);
        }

        // Enable Multisampling
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

        // Set Double Buffering
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // Set OpenGL Version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        int majorVersion, minorVersion;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
        MKR_CORE_INFO("OpenGL Version: {}.{}", majorVersion, minorVersion);
    }

    void sdl_api::exit_video() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void sdl_api::init_image() {
        int flags = IMG_INIT_JPG | IMG_INIT_PNG;
        if (flags != (flags & IMG_Init(flags))) {
            const std::string err_msg = "IMG_Init failed";
            MKR_CORE_ERROR(err_msg);
            throw std::runtime_error(err_msg);
        }
    }

    void sdl_api::exit_image() {
        IMG_Quit();
    }

    void sdl_api::init_input() {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
        if (0 != SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC)) {
            const std::string err_msg = "SDL_INIT_GAMECONTROLLER or SDL_INIT_JOYSTICK or SDL_INIT_HAPTIC failed";
            MKR_CORE_ERROR(err_msg);
            throw std::runtime_error(err_msg);
        }
        SDL_JoystickEventState(SDL_ENABLE);
        SDL_GameControllerEventState(SDL_ENABLE);
    }

    void sdl_api::exit_input() {
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
    }

    void sdl_api::init_events() {
        if (0 != SDL_InitSubSystem(SDL_INIT_EVENTS)) {
            const std::string err_msg = "SDL_INIT_EVENTS failed";
            MKR_CORE_ERROR(err_msg);
            throw std::runtime_error(err_msg);
        }
    }

    void sdl_api::exit_events() {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
} // mkr