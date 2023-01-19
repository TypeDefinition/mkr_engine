#pragma once

#include "maths/matrix.h"
#include "maths/vector3.h"

namespace mkr {
    struct vertex {
        vector3 position_;
        vector3 tex_coord_;
        vector3 normal_;
        vector3 tangent_;
    };

    struct mesh_instance {
        matrix4x4 model_matrix_;
    };
}