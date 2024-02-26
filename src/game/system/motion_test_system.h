#pragma once

#include <log/log.h>
#include "input/input_event.h"
#include "input/input_manager.h"
#include "event/event_listener.h"

namespace mkr {
    class motion_test_system {
    private:
        event_listener input_listener_;

    public:
        motion_test_system() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const motion_event*>(_event);
                if (!e) { return; }

                std::string info = "Position: " + e->position_.to_string() + " Delta: " + e->delta_.to_string();
                log::info(info);
            });
            input_manager::instance().get_event_dispatcher()->add_listener<motion_event>(&input_listener_);
        }

        ~motion_test_system() {
            input_manager::instance().get_event_dispatcher()->remove_listener<motion_event>(&input_listener_);
        }

        void operator()() {}
    };
} // mkr