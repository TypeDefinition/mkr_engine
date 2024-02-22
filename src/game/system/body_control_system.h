#pragma once

#include <maths/vector3.h>
#include "application/application.h"
#include "input/input_manager.h"
#include "event/event_listener.h"
#include "system/system.h"
#include "component/transform_component.h"
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
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e || e->state_ != button_state::pressed) { return; }

                const float velocity = 10.0f;
                if (e->action_ == move_left) {
                    translation_.x_ += application::instance().delta_time() * velocity;
                }
                if (e->action_ == move_right) {
                    translation_.x_ -= application::instance().delta_time() * velocity;
                }
                if (e->action_ == move_forward) {
                    translation_.z_ += application::instance().delta_time() * velocity;
                }
                if (e->action_ == move_backward) {
                    translation_.z_ -= application::instance().delta_time() * velocity;
                }
                if (e->action_ == look_left) {
                    rotation_.y_ += 180.0f * application::instance().delta_time();
                }
                if (e->action_ == look_right) {
                    rotation_.y_ -= 180.0f * application::instance().delta_time();
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
        }

        ~body_control_system() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform_component& _transform, const body_tag& _body) {
            _transform.rotate(quaternion{vector3::y_axis, rotation_.y_ * maths_util::deg2rad});
            rotation_ = vector3::zero;

            auto forward = _transform.forward() * translation_.z_;
            auto left = _transform.left() * translation_.x_;
            _transform.translate(forward + left);
            translation_ = vector3::zero;
        }
    };
} // mkr