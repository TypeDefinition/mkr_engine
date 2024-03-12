#include <SDL2/SDL.h>
#include <log/log.h>
#include "application/application.h"
#include "application/sdl_message_pump.h"
#include "input/input_manager.h"
#include "graphics/renderer/graphics_renderer.h"
#include "graphics/asset_loader/asset_loader.h"
#include "scene/scene_manager.h"

namespace mkr {
    void application::run() {
        init();
        start();
        update();
        stop();
        exit();
    }

    void application::init() {
        // Initialise logging first to allow systems to start logging.
        log::init();

        // Message Pump
        sdl_message_pump::instance().init();

        // Systems
        input_manager::instance().init();
        asset_loader::instance().init();
        graphics_renderer::instance().init();
        scene_manager::instance().init();
    }

    void application::start() {
        // Message Pump
        sdl_message_pump::instance().start();

        // Systems
        graphics_renderer::instance().start();
    }

    void application::update() {
        uint64_t prev_frame_time = 0;
        uint64_t curr_frame_time = SDL_GetPerformanceCounter();

        run_ = true;
        while (run_) {
            prev_frame_time = curr_frame_time;
            curr_frame_time = SDL_GetPerformanceCounter();
            delta_time_ = static_cast<float>(curr_frame_time - prev_frame_time) / static_cast<float>(SDL_GetPerformanceFrequency());
            time_elapsed_ += delta_time_;

            // Systems
            input_manager::instance().update();
            scene_manager::instance().update();
            graphics_renderer::instance().update();
        }
    }

    void application::stop() {
        // Message Pump
        sdl_message_pump::instance().stop();
    }

    void application::exit() {
        // Exit Message Pump
        sdl_message_pump::instance().exit(); // Needs to exit first or else SDL_PollEvent will crash if the other subsystems are shutdown.

        // Exit systems.
        input_manager::instance().exit();
        asset_loader::instance().exit();
        graphics_renderer::instance().exit();
        scene_manager::instance().exit();

        // Destroy Message Pump
        sdl_message_pump::destroy();

        // Destroy systems.
        input_manager::destroy();
        asset_loader::destroy();
        graphics_renderer::destroy();
        scene_manager::destroy();

        // Exit logging last to allow systems to keep logging till the end.
        log::exit();
    }
} // mkr