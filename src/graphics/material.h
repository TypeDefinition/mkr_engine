#pragma once

#include <memory>
#include "graphics/texture.h"
#include "graphics/shader_program.h"

namespace mkr {
    struct material {
        colour albedo_colour_ = colour::white;
        float gloss_ = 1.0f;
        float displacement_scale_ = 0.02f;

        std::shared_ptr<shader_program> shader_;

        std::shared_ptr<texture_2d> texture_albedo_;
        std::shared_ptr<texture_2d> texture_normal_;
        std::shared_ptr<texture_2d> texture_specular_;
        std::shared_ptr<texture_2d> texture_gloss_;
        std::shared_ptr<texture_2d> texture_displacement_;
    };
}