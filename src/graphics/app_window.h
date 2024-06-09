#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>

namespace mkr {
    enum window_flags : uint32_t {
        none = 0,
        fullscreen = 1,
        borderless = 2,
    };

    class app_window {
    private:
        std::string title_;
        uint32_t width_, height_;
        uint32_t flags_;
        SDL_Window* window_;
        SDL_GLContext gl_context_;

    public:
        app_window(const std::string& _title, uint32_t _width, uint32_t _height, uint32_t _flags)
                : title_(_title), width_(_width), height_(_height), flags_(_flags) {
            // Create window.
            window_ = SDL_CreateWindow(title_.c_str(),
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       static_cast<int>(width_), static_cast<int>(height_),
                                       SDL_WINDOW_OPENGL /* | SDL_WINDOW_INPUT_GRABBED */);
            SDL_SetWindowFullscreen(window_, is_fullscreen() ? SDL_TRUE : SDL_FALSE);
            SDL_SetWindowBordered(window_, is_borderless() ? SDL_FALSE : SDL_TRUE);

            // Query actual window size.
            int actual_width, actual_height;
            SDL_GetWindowSize(window_, &actual_width, &actual_height);
            width_ = static_cast<uint32_t>(actual_width);
            height_ = static_cast<uint32_t>(actual_height);

            // Create OpenGL context.
            gl_context_ = SDL_GL_CreateContext(window_);

            // Initialise glew.
            if (GLEW_OK != glewInit()) {
                throw std::runtime_error("glewInit failed");
            }
        }

        ~app_window() {
            SDL_DestroyWindow(window_);
            SDL_GL_DeleteContext(gl_context_);
        }

        [[nodiscard]] inline bool is_fullscreen() const { return flags_ & fullscreen; }

        [[nodiscard]] inline bool is_borderless() const { return flags_ & borderless; }

        [[nodiscard]] inline const std::string& title() const { return title_; }

        [[nodiscard]] inline uint32_t width() const { return width_; }

        [[nodiscard]] inline uint32_t height() const { return height_; }

        inline void swap_buffers() { SDL_GL_SwapWindow(window_); }
    };
} // mkr