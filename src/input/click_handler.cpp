#include "input/click_handler.h"
#include "input/input_helper.h"

namespace mkr {
    void click_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
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
            click_event e{action, button_state::down, positions_[action]};
            _event_dispatcher.dispatch_event<click_event>(&e);
        }
        for (auto action : curr_state_) {
            click_event e{action, button_state::pressed, positions_[action]};
            _event_dispatcher.dispatch_event<click_event>(&e);
        }
        for (auto action : up_buttons) {
            click_event e{action, button_state::up, positions_[action]};
            _event_dispatcher.dispatch_event<click_event>(&e);
        }

        prev_state_ = curr_state_;
    }

    void click_handler::on_key_down(input_mask_t _input_mask) {
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

    void click_handler::on_key_up(input_mask_t _input_mask) {
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

    void click_handler::on_motion(input_mask_t _input_mask, vector2 _position) {
        std::lock_guard<std::mutex> reg_lock{reg_mutex_};
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& actions = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            for (auto action : actions) {
                positions_[action] = _position;
            }
        }
    }

    void click_handler::register_click(input_action_t _input_action, input_mask_t _input_mask) {
        std::lock_guard lock{reg_mutex_};
        registry_[_input_mask].insert(_input_action);
    }

    void click_handler::unregister_click(input_action_t _input_action, input_mask_t _input_mask) {
        std::lock_guard lock{reg_mutex_};
        registry_[_input_mask].erase(_input_action);
    }
} // mkr