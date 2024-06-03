#pragma once

#include <unordered_map>
#include "event/event_dispatcher.h"
#include "input/input.h"

namespace mkr {
    class axis1d_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, float> state_;

        // Support using buttons as positive/negative axes.
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> positive_buttons_, negative_buttons_;
        std::unordered_map<input_action_t, bool> positive_, negative_;

    public:
        axis1d_handler() = default;
        virtual ~axis1d_handler() = default;

        void dispatch_events(event_dispatcher& _event_dispatcher);

        void on_button_down(input_mask_t _input_mask);
        void on_button_up(input_mask_t _input_mask);
        void on_axis(input_mask_t _input_mask, float _value);

        // Support using buttons as positive/negative axes.
        void register_button(input_action_t _action, input_mask_t _positive_mask, input_mask_t _negative_mask);
        void unregister_button(input_action_t _action, input_mask_t _positive_mask, input_mask_t _negative_mask);
        void register_axis(input_action_t _action, input_mask_t _mask);
        void unregister_axis(input_action_t _action, input_mask_t _mask);
    };
} // mkr