#include "input/keyboard_handler.h"
#include "input/input_helper.h"

namespace mkr {
    void keyboard_handler::on_key_down(input_mask _input_mask) {
        std::lock_guard<std::mutex> key_lock{key_mutex_};
        for (const auto& iter: registered_keys_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& names = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            curr_state_.insert(names.begin(), names.end());
        }
    }

    void keyboard_handler::on_key_up(input_mask _input_mask) {
        std::lock_guard<std::mutex> key_lock{key_mutex_};
        for (const auto& iter: registered_keys_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& names = iter.second;
            std::lock_guard<std::mutex> state_lock{state_mutex_};
            for (auto name: names) {
                curr_state_.erase(name);
            }
        }
    }

    void keyboard_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        std::lock_guard<std::mutex> state_lock{state_mutex_};

        std::unordered_set<input_name> down_buttons = curr_state_;
        for (auto name: prev_state_) {
            down_buttons.erase(name);
        }
        std::unordered_set<input_name> up_buttons = prev_state_;
        for (auto name: curr_state_) {
            up_buttons.erase(name);
        }

        for (auto name: down_buttons) {
            button_event e{name, button_event::button_state::down};
            _event_dispatcher.dispatch_event<button_event>(&e);
        }
        for (auto name: curr_state_) {
            button_event e{name, button_event::button_state::pressed};
            _event_dispatcher.dispatch_event<button_event>(&e);
        }
        for (auto name: up_buttons) {
            button_event e{name, button_event::button_state::up};
            _event_dispatcher.dispatch_event<button_event>(&e);
        }

        prev_state_ = curr_state_;
    }

    void keyboard_handler::register_button(input_name _input_name, input_mask _input_mask) {
        std::lock_guard<std::mutex> key_lock{key_mutex_};
        registered_keys_[_input_mask].insert(_input_name);
    }

    void keyboard_handler::unregister_button(input_name _input_name, input_mask _input_mask) {
        std::lock_guard<std::mutex> key_lock{key_mutex_};
        registered_keys_[_input_mask].erase(_input_name);
    }
}