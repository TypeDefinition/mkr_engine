#pragma once

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "event/event_dispatcher.h"
#include "input/input.h"
#include "input/input_event.h"

namespace mkr {
    class button_handler {
    private:
        std::unordered_map<input_mask_t, std::unordered_set<input_action_t>> registry_;
        std::unordered_set<input_action_t> prev_state_, curr_state_;
        std::mutex reg_mutex_, state_mutex_;

    public:
        button_handler() = default;
        virtual ~button_handler() = default;

        void dispatch_events(event_dispatcher& _event_dispatcher);
        void on_key_down(input_mask_t _input_mask);
        void on_key_up(input_mask_t _input_mask);
        void register_button(input_action_t _input_action, input_mask_t _input_mask);
        void unregister_button(input_action_t _input_action, input_mask_t _input_mask);
    };
} // mkr