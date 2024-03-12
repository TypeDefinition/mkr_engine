#pragma once

#include "maths/matrix.h"

namespace mkr {
    struct mesh_instance_data {
        matrix4x4 model_matrix_;
        matrix3x3 normal_matrix_;
    };
} // mkr