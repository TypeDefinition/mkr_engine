#include <log/log.h>
#include "application/sdl_message_pump.h"

namespace mkr {
    void sdl_message_pump::init() {
        run_ = dispatch_ = false;
        if (0 != SDL_InitSubSystem(SDL_INIT_EVENTS)) {
            const std::string err_msg = "SDL_INIT_EVENTS failed";
            log::error(err_msg);
            throw std::runtime_error(err_msg);
        }
    }

    void sdl_message_pump::start() {
        run_ = true;
        worker_thread_ = std::thread([&]() {
            while (run_) {
                sdl_event e;
                if (!dispatch_ && SDL_PollEvent(&e.sdl_event_)) {
                    std::lock_guard lock{mutex_};
                    event_queue_.push(e);
                }
            }
        });
    }

    void sdl_message_pump::update() {
        dispatch_ = true; // Allow dispatching to get mutex priority.
        std::lock_guard lock{mutex_};
        while (!event_queue_.empty()) {
            event_dispatcher_.dispatch_event<sdl_event>(&event_queue_.front());
            event_queue_.pop();
        }
        dispatch_ = false;
    }

    void sdl_message_pump::stop() {
        run_ = dispatch_ = false;
        worker_thread_.join();
    }

    void sdl_message_pump::exit() {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
}