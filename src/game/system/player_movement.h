#pragma once

#include "input/input_manager.h"
#include "ecs/system/system.h"
#include "ecs/component/transform.h"
#include "game/tag/game_tags.h"
#include "game/input/game_controls.h"

namespace mkr {
    class player_movement : public system {
    private:
        float velocity_ = 10.0f;
        float rotation_velocity_ = 180.0f * maths_util::deg2rad;

    public:
        player_movement(scene *_scene)
                : system(_scene, "Player Movement System") {}

        virtual ~player_movement() {}

        virtual void on_update(float _delta_time) {
            auto bodies = scene_->entities_with<player_body, transform>();
            for (auto ent: bodies) {
                auto trans = scene_->get_component<transform>(ent);

                if (input_manager::instance().is_button_pressed(move_forward)) {
                    trans->translate(trans->forward() * velocity_ * _delta_time);
                }
                if (input_manager::instance().is_button_pressed(move_backward)) {
                    trans->translate(-trans->forward() * velocity_ * _delta_time);
                }
                if (input_manager::instance().is_button_pressed(move_left)) {
                    trans->translate(trans->left() * velocity_ * _delta_time);
                }
                if (input_manager::instance().is_button_pressed(move_right)) {
                    trans->translate(-trans->left() * velocity_ * _delta_time);
                }

                float angle = input_manager::instance().get_axis_value(look_horizontal) * rotation_velocity_ * _delta_time;
                trans->rotate(quaternion{vector3::y_axis(), angle});
            }

            auto heads = scene_->entities_with<player_head, transform>();
            for (auto ent : heads) {
                auto trans = scene_->get_component<transform>(ent);

                float angle = input_manager::instance().get_axis_value(look_vertical) * rotation_velocity_ * _delta_time;
                trans->rotate(quaternion{vector3::x_axis(), angle});
            }
        }
    };
} // mkr