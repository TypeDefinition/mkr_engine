#include <SDL2/SDL.h>
#include "application/application.h"
#include "graphics/graphics_system.h"

namespace mkr {
    void application::init() {
        graphics_system::instance().init();
    }

    void application::run() {
        uint64_t prev_frame_time = 0;
        uint64_t curr_frame_time = SDL_GetPerformanceCounter();
        while (!quit_) {
            prev_frame_time = curr_frame_time;
            curr_frame_time = SDL_GetPerformanceCounter();
            delta_time_ = static_cast<float>((curr_frame_time - prev_frame_time)) / static_cast<float>(SDL_GetPerformanceFrequency());
            time_elapsed_ += delta_time_;

            // Update systems.
            graphics_system::instance().update();
        }
    }

    void application::exit() {
        graphics_system::instance().exit();
    }
}