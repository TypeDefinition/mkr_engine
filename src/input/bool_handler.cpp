#include "input/bool_handler.h"
#include "input/input_helper.h"
#include "input/input_event.h"

namespace mkr {
    void bool_handler::dispatch_events(event_dispatcher& _dispatcher) {
        for (auto iter : curr_state_) {
            const auto& action = iter.first;
            const auto& state = iter.second;
            if (state && !prev_state_[action]) {
                bool_event e{action, input_state::start};
                _dispatcher.dispatch_event<bool_event>(&e);
            }
        }

        for (auto iter : curr_state_) {
            const auto& action = iter.first;
            const auto& state = iter.second;
            if (state) {
                bool_event e{action, input_state::ongoing};
                _dispatcher.dispatch_event<bool_event>(&e);
            }
        }

        for (auto iter : curr_state_) {
            const auto& action = iter.first;
            const auto& state = iter.second;
            if (!state && prev_state_[action]) {
                bool_event e{action, input_state::end};
                _dispatcher.dispatch_event<bool_event>(&e);
            }
        }

        // Update previous state.
        prev_state_ = curr_state_;
    }

    void bool_handler::on_button_down(input_mask_t _input_mask) {
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            for (auto action : iter.second) {
                curr_state_[action] = true;
            }
        }
    }

    void bool_handler::on_button_up(input_mask_t _input_mask) {
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            for (auto action : iter.second) {
                curr_state_[action] = false;
            }
        }
    }

    void bool_handler::register_bool(input_action_t _action, input_mask_t _input_mask) {
        registry_[_input_mask].insert(_action);
    }

    void bool_handler::unregister_bool(input_action_t _action, input_mask_t _input_mask) {
        registry_[_input_mask].erase(_action);
    }
} // mkr
