#pragma once

#include "maths/vector2.h"
#include "maths/vector3.h"

namespace mkr {
    struct vertex {
        vector3 position_;
        vector3 normal_;
        vector3 tangent_;
        vector2 tex_coord_;
    };
}