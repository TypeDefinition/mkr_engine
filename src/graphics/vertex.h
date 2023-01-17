#pragma once

#include "maths/vector2.h"
#include "maths/vector3.h"

namespace mkr {
    struct vertex {
        vector3 position_;
        vector3 tex_coord_;
        vector3 normal_;
        vector3 tangent_;
    };
}