#pragma once

namespace mkr {
    /**
        \brief
            event(s) contain data that is dispatched by an event_dispatcher,
            and is received by an event_listener that is subscribed to the event_dispatcher.
            Create new event types by inheriting from event.

            Example event(s): InputEvent, AxisEvent, ButtonEvent, ClickEvent
    */
    class event {
    public:
        event() = default;

        virtual ~event() = default;
    };
}