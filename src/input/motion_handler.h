#pragma once

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "event/event_dispatcher.h"
#include "input/input.h"
#include "input/input_event.h"

namespace mkr {
    class motion_handler {
    private:
        struct motion_data { vector2 position_, delta_; };

        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, motion_data> state_;
        std::mutex reg_mutex_, state_mutex_;

    public:
        motion_handler() = default;
        virtual ~motion_handler() = default;

        void dispatch_events(event_dispatcher& _event_dispatcher);
        void on_motion(input_mask_t _input_mask, vector2 _position, vector2 _delta);
        void register_motion(input_action_t _input_action, input_mask_t _input_mask);
        void unregister_motion(input_action_t _input_action, input_mask_t _input_mask);
    };
} // mkr