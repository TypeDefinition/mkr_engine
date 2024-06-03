#include "input/axis1d_handler.h"
#include "input/input_helper.h"
#include "input/input_event.h"

namespace mkr {
    void axis1d_handler::dispatch_events(event_dispatcher& _dispatcher) {
        for (auto& iter : curr_state_) {
            const auto& action = iter.first;
            const auto& value = iter.second;

            if (value != 0.0f && prev_state_[action] == 0.0f) {
                axis1d_event e{action, input_state::start, value};
                _dispatcher.dispatch_event<axis1d_event>(&e);
            }
        }

        for (auto& iter : curr_state_) {
            const auto& action = iter.first;
            const auto& value = iter.second;

            if (value != 0.0f) {
                axis1d_event e{action, input_state::ongoing, value};
                _dispatcher.dispatch_event<axis1d_event>(&e);
            }
        }

        for (auto& iter : curr_state_) {
            const auto& action = iter.first;
            const auto& value = iter.second;

            if (value == 0.0f && prev_state_[action] != 0.0f) {
                axis1d_event e{action, input_state::end, value};
                _dispatcher.dispatch_event<axis1d_event>(&e);
            }
        }

        // TODO: Maybe only do this once every few frames?
        // Remove state if no change.
        auto iter = curr_state_.begin();
        while (iter != curr_state_.end()) {
            const auto& action = iter->first;
            const auto& value = iter->second;

            if (value == 0.0f && prev_state_[action] == 0.0f) {
                iter = curr_state_.erase(iter);
                continue;
            }
            ++iter;
        }

        // Update previous state.
        prev_state_ = curr_state_;

        // Clear current state.
        for (auto& iter : curr_state_) {
            iter.second = 0.0f;
        }
    }

    void axis1d_handler::on_axis(input_mask_t _input_mask, float _value) {
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& actions = iter.second;
            for (auto action : actions) {
                curr_state_[action] += _value;
            }
        }
    }

    void axis1d_handler::register_axis1d(input_action_t _action, input_mask_t _mask) {
        registry_[_mask].insert(_action);
    }

    void axis1d_handler::unregister_axis1d(input_action_t _action, input_mask_t _mask) {
        registry_[_mask].erase(_action);
    }
} // mkr