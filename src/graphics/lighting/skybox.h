#pragma once

#include <maths/colour.h>
#include "graphics/mesh/mesh.h"
#include "graphics/texture/texture.h"
#include "graphics/shader/shader_program.h"

namespace mkr {
    class skybox {
    public:
        colour colour_ = colour::white();
        cubemap* texture_ = nullptr;
        shader_program* shader_ = nullptr;
    };
}