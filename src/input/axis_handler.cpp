#include "input/axis_handler.h"
#include "input/input_helper.h"

namespace mkr {
    void axis_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        std::lock_guard state_lock{state_mutex_};
        for (auto action : downed_positives_) {
            state_[action] += 1.0f;
        }
        for (auto action : downed_negatives_) {
            state_[action] -= 1.0f;
        }
        for (auto iter : state_) {
            axis_event e{iter.first, iter.second};
            _event_dispatcher.dispatch_event<axis_event>(&e);
        }
        state_.clear();
    }

    void axis_handler::on_axis(input_mask_t _input_mask, float _value) {
        std::lock_guard reg_lock{reg_mutex_};
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            std::lock_guard state_lock{state_mutex_};
            for (auto action : actions) {
                state_[action] += _value;
            }
        }
    }

    void axis_handler::register_axis(input_action_t _action, input_mask_t _axis) {
        std::lock_guard lock{reg_mutex_};
        registry_[_axis].insert(_action);
    }

    void axis_handler::unregister_axis(input_action_t _action, input_mask_t _axis) {
        std::lock_guard lock{reg_mutex_};
        registry_[_axis].erase(_action);
    }

    void axis_handler::on_key_down(input_mask_t _input_mask) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        for (const auto& iter : positive_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            downed_positives_.insert(actions.begin(), actions.end());
        }

        for (const auto& iter : negative_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            downed_negatives_.insert(actions.begin(), actions.end());
        }
    }

    void axis_handler::on_key_up(input_mask_t _input_mask) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        for (const auto& iter : positive_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            for (auto action : actions) { downed_positives_.erase(action); }
        }

        for (const auto& iter : negative_buttons_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) { continue; }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            for (auto action : actions) { downed_negatives_.erase(action); }
        }
    }

    void axis_handler::register_button(input_action_t _action, input_mask_t _positive_button, input_mask_t _negative_button) {
        std::lock_guard lock{reg_mutex_};
        positive_buttons_[_positive_button].insert(_action);
        negative_buttons_[_negative_button].insert(_action);
    }

    void axis_handler::unregister_button(input_action_t _action, input_mask_t _positive_button, input_mask_t _negative_button) {
        std::lock_guard lock{reg_mutex_};
        positive_buttons_[_positive_button].erase(_action);
        negative_buttons_[_negative_button].erase(_action);
    }
} // mkr