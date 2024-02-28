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
        std::mutex reg_mutex_, state_mutex_;
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_map<input_action_t, float> state_;

        // Support using buttons as positive/negative axes.
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> positive_buttons_, negative_buttons_;
        std::unordered_set<input_action_t> downed_positives_, downed_negatives_;

    public:
        axis_handler() = default;
        virtual ~axis_handler() = default;

        void dispatch_events(event_dispatcher& _event_dispatcher);
        void on_axis(input_mask_t _input_mask, float _value);
        void register_axis(input_action_t _action, input_mask_t _axis);
        void unregister_axis(input_action_t _action, input_mask_t _axis);

        // Support using buttons as positive/negative axes.
        void on_key_down(input_mask_t _input_mask);
        void on_key_up(input_mask_t _input_mask);
        void register_button(input_action_t _action, input_mask_t _positive_button, input_mask_t _negative_button);
        void unregister_button(input_action_t _action, input_mask_t _positive_button, input_mask_t _negative_button);
    };
} // mkr