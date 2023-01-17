#include "system/systems.h"

namespace mkr {
    void calculate_transforms(flecs::entity _entity, const root& _root) {
        static std::function<void(flecs::entity, matrix4x4, quaternion)> func =
                [&](flecs::entity _e, matrix4x4 _parent_matrix, quaternion _parent_rotation) {
                    if (!_e.has<transform>()) {
                        return;
                    }

                    const auto* trans = _e.get<transform>();
                    const matrix4x4 model_matrix = _parent_matrix * trans->model_matrix();
                    const quaternion rotation = _parent_rotation * trans->get_rotation();
                    const vector3 position{model_matrix[3][0], model_matrix[3][1], model_matrix[3][2]};
                    const vector3 left = quaternion::rotate(vector3::x_axis, rotation);
                    const vector3 up = quaternion::rotate(vector3::y_axis, rotation);
                    const vector3 forward = quaternion::rotate(vector3::z_axis, rotation);

                    _e.set<global_transform>({model_matrix, rotation, position, left, up, forward});
                    _e.children([=](flecs::entity _child) { func(_child, model_matrix, rotation); });
                };

        func(_entity, matrix4x4::identity(), quaternion::identity);
    }
}