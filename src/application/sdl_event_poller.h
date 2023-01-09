#pragma once

#include <thread>
#include <SDL2/SDL.h>
#include <common/singleton.h>
#include "event/event_dispatcher.h"

namespace mkr {
    class sdl_event : public event {
    public:
        SDL_Event sdl_event_;
    };

    class sdl_event_poller : public singleton<sdl_event_poller> {
        friend class singleton<sdl_event_poller>;

    private:
        event_dispatcher event_dispatcher_;
        std::thread worker_thread_;
        std::atomic_bool run_;

        sdl_event_poller() = default;

        virtual ~sdl_event_poller() = default;

    public:
        inline event_dispatcher& get_event_dispatcher() { return event_dispatcher_; }

        inline const event_dispatcher& get_event_dispatcher() const { return event_dispatcher_; }

        void init();

        void start();

        void stop();

        void exit();
    };
}