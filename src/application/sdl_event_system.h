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

    class sdl_event_system : public singleton<sdl_event_system> {
        friend class singleton<sdl_event_system>;

    private:
        event_dispatcher event_dispatcher_;
        std::thread worker_thread_;
        std::atomic_bool terminate_ = false;

        sdl_event_system() = default;

        virtual ~sdl_event_system() = default;

    public:
        inline event_dispatcher* get_event_dispatcher() { return &event_dispatcher_; }

        inline const event_dispatcher* get_event_dispatcher() const { return &event_dispatcher_; }

        void init();

        void run();

        void exit();
    };
}