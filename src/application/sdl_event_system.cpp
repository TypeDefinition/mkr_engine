#include <spdlog/spdlog.h>
#include "application/sdl_event_system.h"

namespace mkr {
    void sdl_event_system::init() {
        if (0 != SDL_InitSubSystem(SDL_INIT_EVENTS)) {
            const std::string err_msg = "SDL_INIT_EVENTS failed";
            spdlog::error(err_msg);
            throw std::runtime_error(err_msg);
        }
    }

    void sdl_event_system::run() {
        worker_thread_ = std::thread([&]() {
            while (!terminate_) {
                sdl_event e;
                if (SDL_PollEvent(&e.sdl_event_)) {
                    event_dispatcher_.dispatch_event<sdl_event>(&e);
                }
            }
        });
    }

    void sdl_event_system::exit() {
        terminate_ = true;
        worker_thread_.join();
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
}