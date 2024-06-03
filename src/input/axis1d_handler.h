#pragma once

#include <unordered_map>
#include "event/event_dispatcher.h"
#include "input/input.h"

namespace mkr {
    class axis1d_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, float> prev_state_, curr_state_;

    public:
        axis1d_handler() = default;
        virtual ~axis1d_handler() = default;

        void dispatch_events(event_dispatcher& _dispatcher);

        void on_axis(input_mask_t _input_mask, float _value);

        void register_axis1d(input_action_t _action, input_mask_t _mask);
        void unregister_axis1d(input_action_t _action, input_mask_t _mask);
    };
} // mkr