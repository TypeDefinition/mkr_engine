#pragma once

#include <functional>
#include "event/event.h"

namespace mkr {
    /**
        \brief Used to subscribe to an event_dispatcher to receive event(s) from that event_dispatcher. It is possible to subscribe to multiple EventDispatchers.
    */
    class event_listener {
    private:
        /// When a event is received, the function assigned to callback_ is invoked. The event will be passed to the function as a parameter.
        std::function<void(const event*)> callback_ = [](const event*) {};

    public:
        event_listener() = default;

        ~event_listener() = default;

        /**
            \brief Sets a function to be called when an event is received.
            \param _callback The callback function to invoke when an event is received.
        */
        void set_callback(const std::function<void(const event*)>& _callback) { callback_ = _callback; }

        /**
            \brief Used by event_dispatcher to invoke the callback and pass the event.
            \param _event The event that is received.
        */
        void invoke_callback(const event* _event) { std::invoke(callback_, _event); }
    };
}