#include "input/button_handler.h"
#include "input/input_helper.h"

namespace mkr {
    void button_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        std::lock_guard<std::mutex> state_lock{state_mutex_};

        std::unordered_set<input_action_t> down_buttons = curr_state_;
        for (auto action : prev_state_) {
            down_buttons.erase(action);
        }
        std::unordered_set<input_action_t> up_buttons = prev_state_;
        for (auto action : curr_state_) {
            up_buttons.erase(action);
        }

        for (auto action : down_buttons) {
            button_event e{action, button_state::down};
            _event_dispatcher.dispatch_event<button_event>(&e);
        }
        for (auto action : curr_state_) {
            button_event e{action, button_state::pressed};
            _event_dispatcher.dispatch_event<button_event>(&e);
        }
        for (auto action : up_buttons) {
            button_event e{action, button_state::up};
            _event_dispatcher.dispatch_event<button_event>(&e);
        }

        prev_state_ = curr_state_;
    }

    void button_handler::on_key_down(input_mask_t _input_mask) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            curr_state_.insert(actions.begin(), actions.end());
        }
    }

    void button_handler::on_key_up(input_mask_t _input_mask) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            for (auto action : actions) {
                curr_state_.erase(action);
            }
        }
    }

    void button_handler::register_button(input_action_t _action, input_mask_t _button) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        registry_[_button].insert(_action);
    }

    void button_handler::unregister_button(input_action_t _action, input_mask_t _button) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        registry_[_button].erase(_action);
    }
} // mkr