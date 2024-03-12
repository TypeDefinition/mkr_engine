#include "scene/scene.h"
#include "application/application.h"
#include "component/transform.h"
#include "component/local_to_world.h"
#include "component/render_mesh.h"
#include "component/light.h"
#include "component/camera.h"

namespace mkr {
    void scene::update() {
        // Update Local-To-World.
        auto q = world_.query_builder<const transform, local_to_world, const local_to_world*>()
            .term_at(3).parent()
            .cascade().optional()
            .build();
        q.iter([](flecs::iter& _iter, const transform* _child, local_to_world* _out, const local_to_world* _parent) {
            for (auto i: _iter) {
                matrix4x4 trans = _child[i].transform_matrix();
                quaternion rot = _child[i].get_rotation();
                if (_parent) {
                    trans = _parent->transform_ * trans;
                    rot = _parent->rotation_ * rot;
                }
                _out[i].transform_ = trans;
                _out[i].rotation_ = rot;
                _out[i].position_ = vector3{trans[3][0], trans[3][1], trans[3][2]};
                _out[i].left_ = quaternion::rotate(vector3::x_axis, rot);
                _out[i].up_ = quaternion::rotate(vector3::y_axis, rot);
                _out[i].forward_ = quaternion::rotate(vector3::z_axis, rot);
            }
        });
        world_.progress(application::instance().delta_time());
    }
} // mkr