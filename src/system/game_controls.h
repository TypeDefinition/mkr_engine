#pragma once

#include <maths/quaternion.h>
#include "event/event_listener.h"
#include "input/input_manager.h"
#include "application/application.h"
#include "component/transform.h"
#include "component/camera.h"

namespace mkr {
    enum controls : input_action {
        quit = 0,

        move_left,
        move_right,
        move_forward,
        move_backward,

        look_left,
        look_right,
        look_up,
        look_down,
    };

    class head_control {
    private:
        event_listener input_listener_;
        vector3 rotation_;

    public:
        head_control() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e) { return; }
                if (e->state_ != button_state::pressed) { return; }

                if (e->action_ == quit) { application::instance().terminate(); }

                if (e->action_ == look_up) {
                    rotation_.x_ -= 180.0f * application::instance().delta_time();
                }
                if (e->action_ == look_down) {
                    rotation_.x_ += 180.0f * application::instance().delta_time();
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
        }

        ~head_control() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform& _transform, const player_head& _head) {
            _transform.rotate(quaternion{vector3::x_axis, rotation_.x_ * maths_util::deg2rad});
            rotation_ = vector3::zero;
        }
    };

    class body_control {
    private:
        event_listener input_listener_;
        vector3 translation_;
        vector3 rotation_;

    public:
        body_control() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e) { return; }
                if (e->state_ != button_state::pressed) { return; }

                float velocity = 10.0f;
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

        ~body_control() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform& _transform, const player_body& _body) {
            _transform.rotate(quaternion{vector3::y_axis, rotation_.y_ * maths_util::deg2rad});
            rotation_ = vector3::zero;

            auto forward = _transform.forward() * translation_.z_;
            auto left = _transform.left() * translation_.x_;
            _transform.translate(forward + left);
            translation_ = vector3::zero;
        }
    };
}