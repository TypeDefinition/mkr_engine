#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class shadow_2d_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Vertex Shader
            u_view_matrix,
            u_projection_matrix,

            u_texture_offset,
            u_texture_scale,

            // Fragment Shader
            u_is_transparent,
            u_diffuse_colour,
            u_texture_diffuse_enabled,
            u_texture_diffuse,

            num_shader_uniforms,
        };

    protected:
        void assign_uniforms() {
            // Vertex Shader
            uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
            uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");

            uniform_handles_[uniform::u_texture_offset] = get_uniform_location("u_texture_offset");
            uniform_handles_[uniform::u_texture_scale] = get_uniform_location("u_texture_scale");

            // Fragment Shader
            uniform_handles_[uniform::u_is_transparent] = get_uniform_location("u_is_transparent");
            uniform_handles_[uniform::u_diffuse_colour] = get_uniform_location("u_diffuse_colour");
            uniform_handles_[uniform::u_texture_diffuse_enabled] = get_uniform_location("u_texture_diffuse_enabled");
            uniform_handles_[uniform::u_texture_diffuse] = get_uniform_location("u_texture_diffuse");
        }

        void assign_textures() {
            set_uniform(uniform::u_texture_diffuse, (int32_t) texture_unit::texture_diffuse);
        }

    public:
        shadow_2d_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
            : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
            assign_textures();
        }

        ~shadow_2d_shader() override = default;
    };
}