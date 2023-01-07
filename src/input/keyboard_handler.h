#pragma once

#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "event/event_dispatcher.h"
#include "input/input.h"
#include "input/input_event.h"

namespace mkr {
    class keyboard_handler {
    private:
        std::unordered_map<input_mask, std::unordered_set<input_name>> registered_keys_;
        std::unordered_set<input_name> prev_state_, curr_state_;
        std::mutex key_mutex_, state_mutex_;

    public:
        keyboard_handler() = default;

        ~keyboard_handler() = default;

        void on_key_down(input_mask _input_mask);

        void on_key_up(input_mask _input_mask);

        void dispatch_events(event_dispatcher& _event_dispatcher);

        void register_button(input_name _input_name, input_mask _input_mask);

        void unregister_button(input_name _input_name, input_mask _input_mask);
    };
}