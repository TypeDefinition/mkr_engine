#include "input/motion_handler.h"
#include "input/input_helper.h"

namespace mkr {
    void motion_handler::dispatch_events(event_dispatcher& _event_dispatcher) {
        std::lock_guard state_lock{state_mutex_};
        for (auto iter : state_) {
            motion_event e{iter.first, iter.second.position_, iter.second.delta_};
            _event_dispatcher.dispatch_event<motion_event>(&e);
        }
        state_.clear();
    }

    void motion_handler::on_motion(input_mask_t _input_mask, vector2 _position, vector2 _delta) {
        std::lock_guard reg_lock{reg_mutex_};
        for (const auto& iter : registry_) {
            if (!input_helper::compare_mask(_input_mask, iter.first)) {
                continue;
            }

            const auto& actions = iter.second;
            std::lock_guard state_lock{state_mutex_};
            for (auto action : actions) {
                auto& m = state_[action];
                m.position_ = _position;
                m.delta_ += _delta;
            }
        }
    }

    void motion_handler::register_motion(input_action_t _input_action, input_mask_t _input_mask) {
        std::lock_guard lock{reg_mutex_};
        registry_[_input_mask].insert(_input_action);
    }

    void motion_handler::unregister_motion(input_action_t _input_action, input_mask_t _input_mask) {
        std::lock_guard lock{reg_mutex_};
        registry_[_input_mask].erase(_input_action);
    }
} // mkr