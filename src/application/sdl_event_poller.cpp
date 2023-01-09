#include "log/log.h"
#include "application/sdl_event_poller.h"

namespace mkr {
    void sdl_event_poller::init() {
        if (0 != SDL_InitSubSystem(SDL_INIT_EVENTS)) {
            const std::string err_msg = "SDL_INIT_EVENTS failed";
            log::error(err_msg);
            throw std::runtime_error(err_msg);
        }
    }

    void sdl_event_poller::start() {
        run_ = true;
        worker_thread_ = std::thread([&]() {
            sdl_event e;
            while (run_) {
                if (SDL_PollEvent(&e.sdl_event_)) {
                    event_dispatcher_.dispatch_event<sdl_event>(&e);
                }
            }
        });
    }

    void sdl_event_poller::stop() {
        run_ = false;
        worker_thread_.join();
    }

    void sdl_event_poller::exit() {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
}