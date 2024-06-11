#pragma once

#include "application/application.h"
#include "event/event_listener.h"
#include "input/input_manager.h"
#include "input/input_event.h"
#include "ecs/system/system.h"
#include "ecs/component/transform.h"
#include "game/tag/game_tags.h"
#include "game/input/game_controls.h"

namespace mkr {
    class player_movement : public system {
    private:
        float velocity_ = 10.0f;
        float rotation_velocity_ = maths_util::deg2rad * 20.0f;
        event_listener bool_listener_, axis1d_listener_;

        void on_bool_event(const bool_event* _event) {
            if (_event->action_ != move_forward &&
                _event->action_ != move_backward &&
                _event->action_ != move_left &&
                _event->action_ != move_right) {
                return;
            }

            const float delta_time = application::instance().delta_time();
            auto bodies = scene_->entities_with<player_body, transform>();
            for (auto ent: bodies) {
                auto trans = scene_->get_component<transform>(ent);
                if (_event->action_ == move_forward) {
                    trans->translate(trans->forward() * velocity_ * delta_time);
                }
                if (_event->action_ == move_backward) {
                    trans->translate(-trans->forward() * velocity_ * delta_time);
                }
                if (_event->action_ == move_left) {
                    trans->translate(trans->left() * velocity_ * delta_time);
                }
                if (_event->action_ == move_right) {
                    trans->translate(-trans->left() * velocity_ * delta_time);
                }
            }
        }

        void on_axis1d_event(const axis1d_event* _event) {
            if (_event->action_ == look_horizontal) {
                const float delta_time = application::instance().delta_time();
                auto entities = scene_->entities_with<player_body, transform>();
                for (auto ent: entities) {
                    float angle = -_event->value_ * rotation_velocity_ * delta_time;
                    auto trans = scene_->get_component<transform>(ent);
                    trans->rotate(quaternion{vector3::y_axis(), angle});
                }
            }

            if (_event->action_ == look_vertical) {
                const float delta_time = application::instance().delta_time();
                auto entities = scene_->entities_with<player_head, transform>();
                for (auto ent: entities) {
                    float angle = _event->value_ * rotation_velocity_ * delta_time;
                    auto trans = scene_->get_component<transform>(ent);
                    trans->rotate(quaternion{vector3::x_axis(), angle});
                }
            }
        }

    public:
        player_movement(scene* _scene)
                : system(_scene, "Player Movement System") {
            bool_listener_.set_callback<bool_event>(std::bind(&player_movement::on_bool_event, this, std::placeholders::_1));
            axis1d_listener_.set_callback<axis1d_event>(std::bind(&player_movement::on_axis1d_event, this, std::placeholders::_1));
            input_manager::instance().get_event_dispatcher()->add_listener<bool_event>(&bool_listener_);
            input_manager::instance().get_event_dispatcher()->add_listener<axis1d_event>(&axis1d_listener_);
        }

        virtual ~player_movement() {
            input_manager::instance().get_event_dispatcher()->remove_listener<bool_event>(&bool_listener_);
            input_manager::instance().get_event_dispatcher()->remove_listener<axis1d_event>(&axis1d_listener_);
        }
    };
} // mkr