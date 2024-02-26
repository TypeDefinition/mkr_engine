#include "input/axis_handler.h"
#include "input/input_helper.h"

namespace mkr {
    void axis_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        std::lock_guard state_lock{state_mutex_};
        for (auto iter : state_) {
            axis_event e{iter.first, iter.second};
            _event_dispatcher.dispatch_event<axis_event>(&e);
        }
        state_.clear();
    }

    void axis_handler::on_axis(input_mask_t _input_mask, float _value) {
        std::lock_guard reg_lock{reg_mutex_};
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& actions = iter.second;
            std::lock_guard state_lock{state_mutex_};
            for (auto action : actions) {
                state_[action] += _value;
            }
        }
    }

    void axis_handler::register_axis(input_action_t _input_action, input_mask_t _input_mask) {
        std::lock_guard lock{reg_mutex_};
        registry_[_input_mask].insert(_input_action);
    }

    void axis_handler::unregister_axis(input_action_t _input_action, input_mask_t _input_mask) {
        std::lock_guard lock{reg_mutex_};
        registry_[_input_mask].erase(_input_action);
    }
} // mkr