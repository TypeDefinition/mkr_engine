#include <SDL2/SDL.h>
#include <log/log.h>
#include "input/input_system.h"
#include "graphics/graphics_system.h"
#include "application/application.h"
#include "application/sdl_event_system.h"

namespace mkr {
    void application::init() {
        log::init(); // Initialise first to allow systems to start logging.
        sdl_event_system::instance().init();
        input_system::instance().init();
        graphics_system::instance().init();
    }

    void application::run() {
        sdl_event_system::instance().run();

        // Update systems.
        uint64_t prev_frame_time = 0;
        uint64_t curr_frame_time = SDL_GetPerformanceCounter();
        while (!terminate_) {
            prev_frame_time = curr_frame_time;
            curr_frame_time = SDL_GetPerformanceCounter();
            delta_time_ = static_cast<float>(curr_frame_time - prev_frame_time) / static_cast<float>(SDL_GetPerformanceFrequency());
            time_elapsed_ += delta_time_;

            input_system::instance().update();
            graphics_system::instance().update();
        }
    }

    void application::exit() {
        sdl_event_system::instance().exit(); // Needs to exit first or else SDL_PollEvent will crash if the other subsystems are shutdown.
        input_system::instance().exit();
        graphics_system::instance().exit();
        log::exit();  // Exit last to allow systems to keep logging.
    }
}