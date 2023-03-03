#pragma once

#include <maths/quaternion.h>
#include "event/event_listener.h"
#include "input/input_manager.h"
#include "application/application.h"
#include "component/transform.h"
#include "component/camera.h"
#include "graphics/asset_loader.h"

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

        debug_mode_off,
        debug_mode_position,
        debug_mode_normal,
        debug_mode_albedo,
        debug_mode_specular,
        debug_mode_gloss,
        debug_mode_material,
    };

    struct debug_control {
        event_listener input_listener_;
        std::shared_ptr<shader_program> gshader_, lshader_;
        std::shared_ptr<material> materials_1_[8];
        std::shared_ptr<material> materials_2_[8];

        debug_control() {
            for (auto i = 0; i < 8; ++i) {
                materials_1_[i] = asset_loader::instance().make_material("empty" + std::to_string(i));
            }

            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e) { return; }
                if (e->state_ != button_state::down) { return; }

                if (e->action_ == debug_mode_off) {
                    gshader_->set_uniform("u_debug_mode", 0);
                    lshader_->set_uniform("u_debug_mode", 0);
                }
                if (e->action_ == debug_mode_position) {
                    gshader_->set_uniform("u_debug_mode", 0);
                    lshader_->set_uniform("u_debug_mode", 1);
                }
                if (e->action_ == debug_mode_normal) {
                    gshader_->set_uniform("u_debug_mode", 1);
                    lshader_->set_uniform("u_debug_mode", 2);
                }
                if (e->action_ == debug_mode_albedo) {
                    gshader_->set_uniform("u_debug_mode", 3);
                    lshader_->set_uniform("u_debug_mode", 3);
                }
                if (e->action_ == debug_mode_specular) {
                    gshader_->set_uniform("u_debug_mode", 4);
                    lshader_->set_uniform("u_debug_mode", 4);
                }
                if (e->action_ == debug_mode_gloss) {
                    gshader_->set_uniform("u_debug_mode", 5);
                    lshader_->set_uniform("u_debug_mode", 5);
                }
                if (e->action_ == debug_mode_material) {
                    for (auto i = 0; i < 8; ++i) {
                        materials_1_[i]->texture_albedo_.swap(materials_2_[i]->texture_albedo_);
                        materials_1_[i]->texture_normal_.swap(materials_2_[i]->texture_normal_);
                        materials_1_[i]->texture_displacement_.swap(materials_2_[i]->texture_displacement_);
                        materials_1_[i]->texture_gloss_.swap(materials_2_[i]->texture_gloss_);
                        materials_1_[i]->texture_specular_.swap(materials_2_[i]->texture_specular_);
                    }
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
        }

        ~debug_control() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }
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

                if (e->action_ == debug_mode_off) {

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