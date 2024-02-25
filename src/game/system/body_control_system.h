#pragma once

#include <maths/vector3.h>
#include "application/application.h"
#include "input/input_manager.h"
#include "event/event_listener.h"
#include "system/system.h"
#include "component/transform.h"
#include "game/tag/tag.h"
#include "game/input/game_controls.h"

namespace mkr {
    class body_control_system {
    private:
        event_listener input_listener_;
        vector3 rotation_, translation_;

    public:
        body_control_system() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* be = dynamic_cast<const button_event*>(_event);
                if (be && be->state_ == button_state::pressed) {
                    const float velocity = 10.0f;
                    if (be->action_ == move_left) { translation_.x_ += application::instance().delta_time() * velocity; }
                    if (be->action_ == move_right) { translation_.x_ -= application::instance().delta_time() * velocity; }
                    if (be->action_ == move_forward) { translation_.z_ += application::instance().delta_time() * velocity; }
                    if (be->action_ == move_backward) { translation_.z_ -= application::instance().delta_time() * velocity; }
                    if (be->action_ == look_left) { rotation_.y_ += 180.0f * application::instance().delta_time(); }
                    if (be->action_ == look_right) { rotation_.y_ -= 180.0f * application::instance().delta_time(); }
                }

                const auto* ae = dynamic_cast<const axis_event*>(_event);
                if (ae) {
                    if (ae->action_ == look_horizontal) { rotation_.y_ -= ae->value_/10.0f; }
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
            input_manager::instance().get_event_dispatcher()->add_listener<axis_event>(&input_listener_);
        }

        ~body_control_system() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform& _transform, const body_tag& _body) {
            _transform.rotate(quaternion{vector3::y_axis, rotation_.y_ * maths_util::deg2rad});
            rotation_ = vector3::zero;

            auto forward = _transform.forward() * translation_.z_;
            auto left = _transform.left() * translation_.x_;
            _transform.translate(forward + left);
            translation_ = vector3::zero;
        }
    };
} // mkr