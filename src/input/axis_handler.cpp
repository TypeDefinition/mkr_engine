#include "input/axis1d_handler.h"
#include "input/input_helper.h"
#include "input/input_event.h"

namespace mkr {
    void axis1d_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        for (auto iter : state_) {
            auto& action = iter.first;
            auto& value = iter.second;

            if (value != 0.0f) {
                axis1d_event e{action, value};
                _event_dispatcher.dispatch_event<axis1d_event>(&e);
            }

            value = positive_[action] ? 1.0f : 0.0f;
            value += negative_[action] ? -1.0f : 0.0f;
        }
    }

    void axis1d_handler::on_axis(input_mask_t _input_mask, float _value) {
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            for (auto action : actions) { state_[action] += _value; }
        }
    }

    void axis1d_handler::on_button_down(input_mask_t _input_mask) {
        for (const auto& iter : positive_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            for (auto action : iter.second) {
                if (!positive_[action]) {
                    positive_[action] = true;
                    state_[action] += 1.0f;
                }
            }
        }

        for (const auto& iter : negative_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            for (auto action : iter.second) {
                if (!negative_[action]) {
                    negative_[action] = true;
                    state_[action] -= 1.0f;
                }
            }
        }
    }

    void axis1d_handler::on_button_up(input_mask_t _input_mask) {
        for (const auto& iter : positive_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            for (auto action : iter.second) { positive_[action] = false; }
        }

        for (const auto& iter : negative_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            for (auto action : iter.second) { negative_[action] = false; }
        }
    }

    void axis1d_handler::register_axis(input_action_t _action, input_mask_t _mask) {
        registry_[_mask].insert(_action);
    }

    void axis1d_handler::unregister_axis(input_action_t _action, input_mask_t _mask) {
        registry_[_mask].erase(_action);
    }

    void axis1d_handler::register_button(input_action_t _action, input_mask_t _positive_mask, input_mask_t _negative_mask) {
        positive_buttons_[_positive_mask].insert(_action);
        negative_buttons_[_negative_mask].insert(_action);
    }

    void axis1d_handler::unregister_button(input_action_t _action, input_mask_t _positive_mask, input_mask_t _negative_mask) {
        positive_buttons_[_positive_mask].erase(_action);
        negative_buttons_[_negative_mask].erase(_action);
    }
} // mkr
