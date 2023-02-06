#pragma once

#include <memory>
#include "graphics/texture.h"
#include "graphics/shader_program.h"
#include "graphics/render_path.h"

namespace mkr {
    class material {
    public:
        material() = default;
        ~material() = default;

        static bool enable_lights_;
        static std::shared_ptr<shader_program> g_shader_, l_shader_;
        static std::vector<std::shared_ptr<shader_program>> p_shaders_;

        render_path render_path_ = render_path::deferred;
        std::shared_ptr<shader_program> f_shader_; // Only used for forward rendering.

        colour albedo_colour_ = colour::white;
        float gloss_ = 1.0f;
        float displacement_scale_ = 0.02f;

        std::shared_ptr<texture_2d> texture_albedo_;
        std::shared_ptr<texture_2d> texture_normal_;
        std::shared_ptr<texture_2d> texture_specular_;
        std::shared_ptr<texture_2d> texture_gloss_;
        std::shared_ptr<texture_2d> texture_displacement_;
    };
}