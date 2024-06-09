#include <log/log.h>
#include "application/sdl_message_pump.h"

namespace mkr {
    void sdl_message_pump::update() {
        // SDL_PollEvent must be invoked on the main thread according to the specifications.
        // Otherwise, Windows will think that the application is not responding.
        sdl_event e;
        while (SDL_PollEvent(&e.sdl_event_)) {
            event_dispatcher_.dispatch_event<sdl_event>(&e);
        }
    }
}