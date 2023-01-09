#pragma once

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader_program.h"

namespace mkr {
    struct mesh_renderer {
        std::shared_ptr<mesh> mesh_;
        std::shared_ptr<texture_2d> texture_2d_;
        std::shared_ptr<shader_program> shader_;
    };
}