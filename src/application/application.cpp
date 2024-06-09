#include <SDL2/SDL.h>
#include <log/log.h>
#include "sdl/sdl_api.h"
#include "application/application.h"
#include "application/sdl_message_pump.h"
#include "input/input_manager.h"
#include "graphics/renderer/graphics_renderer.h"
#include "graphics/texture/texture_loader.h"
#include "game/scene/demo_scene.h"

namespace mkr {
    void application::run() {
        init();
        update();
        exit();
    }

    void application::init() {
        // Initialise logging first to allow systems to start logging.
        log::init();

        sdl_api::init();

        // Systems
        input_manager::instance().init();
        graphics_renderer::instance().init();
        graphics_renderer::instance().start();

        scene_ = std::make_unique<demo_scene>();
    }

    void application::update() {
        uint64_t prev_frame_time = 0;
        uint64_t curr_frame_time = SDL_GetPerformanceCounter();

        scene_->init();

        run_ = true;
        while (run_) {
            prev_frame_time = curr_frame_time;
            curr_frame_time = SDL_GetPerformanceCounter();
            delta_time_ = static_cast<float>(curr_frame_time - prev_frame_time) / static_cast<float>(SDL_GetPerformanceFrequency());
            time_elapsed_ += delta_time_;

            // Message Pump
            sdl_message_pump::instance().update();

            // Systems
            input_manager::instance().update();
            scene_->update(delta_time_);
            graphics_renderer::instance().update();
        }

        scene_->exit();
    }

    void application::exit() {
        sdl_api::exit();

        // Exit systems.
        input_manager::instance().exit();
        graphics_renderer::instance().exit();

        // Destroy message pump.
        sdl_message_pump::destroy();

        // Destroy systems.
        input_manager::destroy();
        graphics_renderer::destroy();

        // Exit logging last to allow systems to keep logging till the end.
        log::exit();
    }
} // mkr