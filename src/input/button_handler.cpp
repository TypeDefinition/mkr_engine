#include "input/button_handler.h"
#include "input/input_helper.h"
#include "input/input_event.h"

namespace mkr {
    bool button_handler::is_button_down(input_action_t _action) const {
        if (!prev_state_.contains(_action) || !curr_state_.contains(_action)) {
            return false;
        }
        return !prev_state_.find(_action)->second && curr_state_.find(_action)->second;
    }

    bool button_handler::is_button_pressed(input_action_t _action) const {
        if (!curr_state_.contains(_action)) {
            return false;
        }
        return curr_state_.find(_action)->second;
    }

    bool button_handler::is_button_up(input_action_t _action) const {
        if (!prev_state_.contains(_action) || !curr_state_.contains(_action)) {
            return false;
        }
        return prev_state_.find(_action)->second && !curr_state_.find(_action)->second;
    }

    void button_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        for (auto iter : curr_state_) {
            auto action = iter.first;
            auto state = iter.second;
            if (state && !prev_state_[action]) {
                button_event e{action, button_state::down};
                _event_dispatcher.dispatch_event<button_event>(&e);
            }
        }

        for (auto iter : curr_state_) {
            auto action = iter.first;
            auto state = iter.second;
            if (state) {
                button_event e{action, button_state::pressed};
                _event_dispatcher.dispatch_event<button_event>(&e);
            }
        }

        for (auto iter : curr_state_) {
            auto action = iter.first;
            auto state = iter.second;
            if (!state && prev_state_[action]) {
                button_event e{action, button_state::down};
                _event_dispatcher.dispatch_event<button_event>(&e);
            }
        }

        prev_state_ = curr_state_;
    }

    void button_handler::on_button_down(input_mask_t _input_mask) {
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            for (auto action : iter.second) { curr_state_[action] = true; }
        }
    }

    void button_handler::on_button_up(input_mask_t _input_mask) {
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            for (auto action : iter.second) { curr_state_[action] = false; }
        }
    }

    void button_handler::register_button(input_action_t _action, input_mask_t _input_mask) {
        registry_[_input_mask].insert(_action);
    }

    void button_handler::unregister_button(input_action_t _action, input_mask_t _input_mask) {
        registry_[_input_mask].erase(_action);
    }
} // mkr
