#pragma once

#include <maths/colour.h>
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader_program.h"

namespace mkr {
    class skybox {
    public:
        std::shared_ptr<texture_cube> texture_;
        std::shared_ptr<shader_program> shader_;
        colour sky_colour_ = colour::white;
    };
}