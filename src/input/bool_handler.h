#pragma once

#include <unordered_map>
#include "event/event_dispatcher.h"
#include "input/input.h"

namespace mkr {
    class bool_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, bool> prev_state_, curr_state_;

    public:
        bool_handler() = default;
        virtual ~bool_handler() = default;

        void dispatch_events(event_dispatcher& _dispatcher);

        void on_button_down(input_mask_t _input_mask);
        void on_button_up(input_mask_t _input_mask);

        void register_bool(input_action_t _action, input_mask_t _input_mask);
        void unregister_bool(input_action_t _action, input_mask_t _input_mask);
    };
} // mkr