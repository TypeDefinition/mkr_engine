#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>
#include <common/singleton.h>
#include <maths/colour.h>
#include "graphics/app_window.h"
#include "graphics/renderer.h"

namespace mkr {
    class graphics_system : public singleton<graphics_system> {
        friend class singleton<graphics_system>;

    private:
        std::unique_ptr<app_window> app_window_;
        std::unique_ptr<renderer> renderer_;

        graphics_system() {}
        ~graphics_system() {}

    public:
        void init() {
            // Initialise SDL_image.
            int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP;
            if (imgFlags != (IMG_Init(imgFlags) & imgFlags)) {
                const std::string err_msg = "IMG_Init failed";
                spdlog::error(err_msg);
                throw std::runtime_error(err_msg);
            }

            // Initialise SDL video subsystem.
            if (0 != SDL_InitSubSystem(SDL_INIT_VIDEO)) {
                const std::string err_msg = "SDL_INIT_VIDEO failed";
                spdlog::error(err_msg);
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
            spdlog::info("OpenGL Version: {}.{}", majorVersion, minorVersion);

            // Create Window
            app_window_ = std::make_unique<app_window>("mkr_engine", 1920, 1080, window_flags::none);

            // Initialise glew.
            if (GLEW_OK != glewInit()) {
                throw std::runtime_error("glewInit failed");
            }

            renderer_ = std::make_unique<renderer>();
        }

        void update() {
            // Render.
            renderer_->update();

            // Swap buffer.
            app_window_->swap_buffers();
        }

        void exit() {
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            IMG_Quit();
        }
    };
}