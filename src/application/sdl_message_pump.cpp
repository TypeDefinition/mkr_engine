#include <log/log.h>
#include "application/sdl_message_pump.h"

namespace mkr {
    void sdl_message_pump::init() {
        if (0 != SDL_InitSubSystem(SDL_INIT_EVENTS)) {
            const std::string err_msg = "SDL_INIT_EVENTS failed";
            log::error(err_msg);
            throw std::runtime_error(err_msg);
        }
    }


    void sdl_message_pump::update() {
        // SDL_PollEvent must be invoked on the main thread according to the specifications.
        // Otherwise, Windows will think that the application is not responding.
        sdl_event e;
        while (SDL_PollEvent(&e.sdl_event_)) {
            event_dispatcher_.dispatch_event<sdl_event>(&e);
        }
    }

    void sdl_message_pump::exit() {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
}