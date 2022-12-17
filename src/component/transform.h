#pragma once

#include <maths/vector3.h>
#include <maths/quaternion.h>

namespace mkr {
    struct transform {
        vector3 local_position_;
        vector3 local_scale_ = vector3{1.0f, 1.0f, 1.0f};
        quaternion local_rotation_;
    };
}