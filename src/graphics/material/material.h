#pragma once

#include <memory>
#include "maths/vector2.h"
#include "graphics/texture/texture.h"
#include "graphics/shader/shader_program.h"
#include "graphics/material/render_path.h"

namespace mkr {
    class material {
    public:
        material() = default;
        ~material() = default;

        render_path render_path_ = render_path::deferred;
        shader_program* forward_shader_; // Forward Shading (Per-Material)
        static shader_program* geometry_shader_; // Deferred Shading (Shared)
        static shader_program* light_shader_; // Deferred Shading (Shared)
        static std::vector<shader_program*> post_proc_shaders_; // Post-Processing (Shared)
        static shader_program* shadow_shader_2d_;
        static shader_program* shadow_shader_cube_;

        // Phong Shading
        colour diffuse_colour_ = colour::white();
        colour specular_colour_ = colour::white();
        float gloss_ = 32.0f;
        float displacement_scale_ = 0.05f;

        // Textures
        texture2d* texture_diffuse_;
        texture2d* texture_specular_;
        texture2d* texture_gloss_;
        texture2d* texture_normal_;
        texture2d* texture_displacement_;
        vector2 texture_offset_ = vector2::zero();
        vector2 texture_scale_ = vector2{1.0f, 1.0f};
    };
}