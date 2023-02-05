#pragma once

#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/shader_program.h"

namespace mkr {
    struct mesh_renderer {
        std::shared_ptr<mesh> mesh_;
        std::shared_ptr<texture_2d> texture_albedo_, texture_normal_, texture_specular_, texture_gloss, texture_displacement_;
        std::shared_ptr<shader_program> shader_;
        colour albedo_colour_ = colour::white;
        float gloss_ = 0.3f;
        float displacement_scale_ = 0.1f;
    };
}