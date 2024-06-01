#pragma once

#include <unordered_map>
#include "event/event_dispatcher.h"
#include "input/input.h"

namespace mkr {
    class button_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, bool> prev_state_, curr_state_;

    public:
        button_handler() = default;
        virtual ~button_handler() = default;

        bool is_button_down(input_action_t _action) const;
        bool is_button_pressed(input_action_t _action) const;
        bool is_button_up(input_action_t _action) const;

        void dispatch_events(event_dispatcher& _event_dispatcher);
        void on_button_down(input_mask_t _input_mask);
        void on_button_up(input_mask_t _input_mask);
        void register_button(input_action_t _action, input_mask_t _input_mask);
        void unregister_button(input_action_t _action, input_mask_t _input_mask);
    };
} // mkr