#include <SDL2/SDL.h>
#include <log/log.h>
#include "application/application.h"
#include "application/sdl_event_poller.h"
#include "input/input_manager.h"
#include "graphics/renderer.h"
#include "scene/scene_manager.h"

namespace mkr {
    void application::init() {
        // Initialise logging first to allow systems to start logging.
        log::init();

        // Initialise systems.
        sdl_event_poller::instance().init();
        input_manager::instance().init();
        renderer::instance().init();
        scene_manager::instance().init();
    }

    void application::run() {
        // Start systems.
        sdl_event_poller::instance().start();
        renderer::instance().start();

        // Update systems.
        uint64_t prev_frame_time = 0;
        uint64_t curr_frame_time = SDL_GetPerformanceCounter();
        while (!terminate_) {
            prev_frame_time = curr_frame_time;
            curr_frame_time = SDL_GetPerformanceCounter();
            delta_time_ = static_cast<float>(curr_frame_time - prev_frame_time) / static_cast<float>(SDL_GetPerformanceFrequency());
            time_elapsed_ += delta_time_;

            input_manager::instance().update();
            scene_manager::instance().update();
            renderer::instance().update();
        }

        // Stop systems.
        sdl_event_poller::instance().stop();
    }

    void application::exit() {
        // Exit systems.
        sdl_event_poller::instance().exit(); // Needs to exit first or else SDL_PollEvent will crash if the other subsystems are shutdown.
        input_manager::instance().exit();
        renderer::instance().exit();
        scene_manager::instance().exit();

        // Destroy systems.
        sdl_event_poller::destroy();
        input_manager::destroy();
        renderer::destroy();
        scene_manager::destroy();

        // Exit logging last to allow systems to keep logging till the end.
        log::exit();
    }
}