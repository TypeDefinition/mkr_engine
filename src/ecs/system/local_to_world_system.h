#pragma once

#include "ecs/system/system.h"
#include "ecs/scene/scene.h"
#include "ecs/component/transform.h"
#include "ecs/component/local_to_world.h"

namespace mkr {
    class local_to_world_system : public system {
    private:
        void update_local_to_world(ecs_id_t _ent, matrix4x4 _parent_trans, quaternion _parent_rot) {
            auto trans = scene_->get_component<transform>(_ent);
            matrix4x4 transform_mat = _parent_trans * trans->transform_matrix();
            quaternion rotation_quat = _parent_rot * trans->get_rotation();

            auto ltw = scene_->get_component<local_to_world>(_ent);
            ltw->transform_ = transform_mat;
            ltw->rotation_ = rotation_quat;
            ltw->position_ = vector3{transform_mat[3][0], transform_mat[3][1], transform_mat[3][2]};
            ltw->left_ = quaternion::rotate(vector3::x_axis(), rotation_quat);
            ltw->up_ = quaternion::rotate(vector3::y_axis(), rotation_quat);
            ltw->forward_ = quaternion::rotate(vector3::z_axis(), rotation_quat);

            auto children = scene_->get_children(_ent);
            for (auto &child: children) {
                update_local_to_world(child, transform_mat, rotation_quat);
            }
        }

    public:
        local_to_world_system(scene *_scene)
                : system(_scene, "Local To World System") {}

        virtual ~local_to_world_system() {}

        virtual void on_update(float _delta_time) {
            auto entities = scene_->entities_with<root>();
            for (auto ent: entities) {
                update_local_to_world(ent, matrix4x4::identity(), quaternion::identity());
            }
        }
    };
}
