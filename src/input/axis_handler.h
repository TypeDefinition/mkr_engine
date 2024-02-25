#pragma once

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "event/event_dispatcher.h"
#include "input/input.h"
#include "input/input_event.h"

namespace mkr {
    class axis_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, float> state_;
        std::mutex reg_mutex_, state_mutex_;

    public:
        axis_handler() = default;
        virtual ~axis_handler() = default;

        void dispatch_events(event_dispatcher& _event_dispatcher);
        void on_axis(input_mask_t _input_mask, float _value);
        void register_axis(input_action_t _input_action, input_mask_t _input_mask);
        void unregister_axis(input_action_t _input_action, input_mask_t _input_mask);
    };
} // mkr