#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <SDL2/SDL.h>
#include <common/singleton.h>
#include "event/event_dispatcher.h"

namespace mkr {
    class sdl_event : public event {
    public:
        SDL_Event sdl_event_;
    };

    class sdl_message_pump : public singleton<sdl_message_pump> {
        friend class singleton<sdl_message_pump>;

    private:
        event_dispatcher event_dispatcher_;

    public:
        sdl_message_pump() = default;
        virtual ~sdl_message_pump() = default;

        inline event_dispatcher& get_event_dispatcher() { return event_dispatcher_; }
        inline const event_dispatcher& get_event_dispatcher() const { return event_dispatcher_; }

        void init();
        void update();
        void exit();
    };
}