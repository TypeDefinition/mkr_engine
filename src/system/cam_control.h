#pragma once

#include <maths/quaternion.h>
#include "event/event_listener.h"
#include "input/input_manager.h"
#include "application/application.h"
#include "component/transform.h"
#include "component/camera.h"
#include "system/input_names.h"

namespace mkr {
    class cam_control {
    private:
        vector3 translation_;
        vector3 rotation_;
        event_listener input_listener_;

    public:
        cam_control() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e) { return; }
                if (e->state_ != button_event::button_state::pressed) { return; }

                if (e->name_ == input_quit) {
                    application::instance().terminate();
                }

                if (e->name_ == input_cam_left) {
                    translation_.x_ += application::instance().delta_time() * 2.0f;
                }
                if (e->name_ == input_cam_right) {
                    translation_.x_ -= application::instance().delta_time() * 2.0f;
                }
                if (e->name_ == input_cam_forward) {
                    translation_.z_ += application::instance().delta_time() * 2.0f;
                }
                if (e->name_ == input_cam_backward) {
                    translation_.z_ -= application::instance().delta_time() * 2.0f;
                }

                if (e->name_ == input_look_left) {
                    rotation_.y_ += 180.0f * application::instance().delta_time();
                }
                if (e->name_ == input_look_right) {
                    rotation_.y_ -= 180.0f * application::instance().delta_time();
                }
                if (e->name_ == input_look_up) {
                    rotation_.x_ -= 180.0f * application::instance().delta_time();
                }
                if (e->name_ == input_look_down) {
                    rotation_.x_ += 180.0f * application::instance().delta_time();
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
        }

        ~cam_control() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform& _transform, camera& _camera) {
            _transform.local_position_ += translation_;
            translation_ = vector3::zero;

            quaternion rot_z{vector3::z_axis, rotation_.z_ * maths_util::deg2rad};
            quaternion rot_y{vector3::y_axis, rotation_.y_ * maths_util::deg2rad};
            quaternion rot_x{vector3::x_axis, rotation_.x_ * maths_util::deg2rad};
            _transform.local_rotation_ = rot_x * rot_y * rot_z * _transform.local_rotation_;
            rotation_ = vector3::zero;
        }
    };
}