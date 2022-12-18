#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
            // Create Window
            window_ = SDL_CreateWindow(title_.c_str(),
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       static_cast<int>(width_), static_cast<int>(height_),
                                       SDL_WINDOW_OPENGL /* | SDL_WINDOW_INPUT_GRABBED */);
            SDL_SetWindowFullscreen(window_, is_fullscreen() ? SDL_TRUE : SDL_FALSE);
            SDL_SetWindowBordered(window_, is_borderless() ? SDL_FALSE : SDL_TRUE);

            // Query Actual Window Size
            int actual_width, actual_height;
            SDL_GetWindowSize(window_, &actual_width, &actual_height);
            width_ = static_cast<uint32_t>(actual_width);
            height_ = static_cast<uint32_t>(actual_height);

            // Create OpenGL Context
            gl_context_ = SDL_GL_CreateContext(window_);
        }

        ~app_window() {
            SDL_DestroyWindow(window_);
            SDL_GL_DeleteContext(gl_context_);
        }

        bool is_fullscreen() const { return flags_ & fullscreen; }

        bool is_borderless() const { return flags_ & borderless; }

        void swap_buffers() { SDL_GL_SwapWindow(window_); }
    };
}