#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <common/family.h>
#include "event/event_listener.h"

namespace mkr {
    /**
        \brief An event_dispatcher is used to dispatch event(s) to event_listener(s) that are subscribed to this dispatcher. This uses the publisher-subscriber pattern.
    */
    class event_dispatcher {
    private:
        /// The event_listener(s) added to this event_dispatcher is sorted by the event type they are subscribed to.
        std::unordered_map<int32_t, std::unordered_set<event_listener*>> listeners_;

    public:
        event_dispatcher() = default;

        ~event_dispatcher() = default;

        /**
            \brief Dispatches an event of type T. Only event_listener(s) subscribed to event(s) of type T will receive the event.
            \param _event The event to dispatch.
        */
        template<typename T>
        void dispatch_event(event* _event) requires std::is_base_of_v<mkr::event, T> {
            for (auto listener : listeners_[family<event>::get_id<T>()]) {
                listener->invoke_callback(_event);
            }
        }

        /**
            \brief Subscribes an event_listener to event(s) of type T from this event_dispatcher. Only event(s) of type T will be received from this event_dispatcher. It is possible use add_listener multiple times to listen for multiple types of event(s).
            \param _listener The event_listener to add.
        */
        template<typename T>
        void add_listener(event_listener* _listener) requires std::is_base_of_v<mkr::event, T> {
            listeners_[family<event>::get_id<T>()].insert(_listener);
        }

        /**
            \brief Unsubscribes an event_listener to event(s) of type T from this event_dispatcher. remove_listener must be called for each event type that the event_listener is subscribed to.
            \param _listener The event_listener to remove.
        */
        template<typename T>
        void remove_listener(event_listener* _listener) requires std::is_base_of_v<mkr::event, T> {
            listeners_[family<event>::get_id<T>()].erase(_listener);
        }
    };
}