#pragma once

#include <maths/colour.h>
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader_program.h"

namespace mkr {
    class skybox {
    public:
        texture_cube* texture_ = nullptr;
        shader_program* shader_ = nullptr;
        colour colour_ = colour::white;
    };
}