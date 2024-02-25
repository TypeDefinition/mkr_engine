#pragma once

#include <log/log.h>
#include <maths/vector3.h>
#include "application/application.h"
#include "input/input_manager.h"
#include "event/event_listener.h"
#include "component/transform.h"
#include "system/system.h"
#include "game/tag/tag.h"
#include "game/input/game_controls.h"

namespace mkr {
    class head_control_system {
    private:
        event_listener input_listener_;
        vector3 rotation_;

    public:
        head_control_system() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* be = dynamic_cast<const button_event*>(_event);
                if (be && be->state_ == button_state::pressed) {
                    if (be->action_ == quit) { application::instance().terminate(); }
                    if (be->action_ == look_up) { rotation_.x_ -= 180.0f * application::instance().delta_time(); }
                    if (be->action_ == look_down) { rotation_.x_ += 180.0f * application::instance().delta_time(); }
                }

                const auto* ae = dynamic_cast<const axis_event*>(_event);
                if (ae) {
                    if (ae->action_ == look_vertical) { rotation_.x_ += ae->value_/10.0f; }
                }

                const auto* ce = dynamic_cast<const click_event*>(_event);
                if (ce) {
                    if (ce->state_ == button_state::down) {
                        if (ce->action_ == test_click) { log::trace("Down: " + ce->position_.to_string()); }
                    }
                    if (ce->state_ == button_state::pressed) {
                        if (ce->action_ == test_click) { log::trace("Pressed: " + ce->position_.to_string()); }
                    }
                    if (ce->state_ == button_state::up) {
                        if (ce->action_ == test_click) { log::trace("Up: " + ce->position_.to_string()); }
                    }
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
            input_manager::instance().get_event_dispatcher()->add_listener<axis_event>(&input_listener_);
            input_manager::instance().get_event_dispatcher()->add_listener<click_event>(&input_listener_);
        }

        ~head_control_system() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform& _transform, const head_tag& _head) {
            _transform.rotate(quaternion{vector3::x_axis, rotation_.x_ * maths_util::deg2rad});
            rotation_ = vector3::zero;
        }
    };
} // mkr