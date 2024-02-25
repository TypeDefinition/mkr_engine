#pragma once

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <maths/vector2.h>
#include "event/event_dispatcher.h"
#include "input/input.h"
#include "input/input_event.h"

namespace mkr {
    class click_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_set<input_action_t> prev_state_, curr_state_;
        std::unordered_map<input_action_t, vector2> positions_;
        std::mutex reg_mutex_, state_mutex_;

    public:
        click_handler() = default;
        virtual ~click_handler() = default;

        void dispatch_events(event_dispatcher& _event_dispatcher);
        void on_key_down(input_mask_t _input_mask);
        void on_key_up(input_mask_t _input_mask);
        void on_motion(input_mask_t _input_mask, vector2 _position);
        void register_click(input_action_t _input_action, input_mask_t _input_mask);
        void unregister_click(input_action_t _input_action, input_mask_t _input_mask);
    };
} // mkr