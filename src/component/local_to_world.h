#pragma once

#include <maths/vector3.h>
#include <maths/quaternion.h>
#include <maths/matrix.h>

namespace mkr {
    struct local_to_world {
        matrix4x4 transform_;
        quaternion rotation_;
        vector3 position_;
        vector3 left_, up_, forward_;
    };
} // mkr