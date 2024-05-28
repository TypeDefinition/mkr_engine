#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class shadow_cubemap_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Vertex Shader
            u_texture_offset,
            u_texture_scale,

            // Geometry Shader
            u_view_projection_matrices0,
            u_view_projection_matrices1,
            u_view_projection_matrices2,
            u_view_projection_matrices3,
            u_view_projection_matrices4,
            u_view_projection_matrices5,

            // Fragment Shader
            u_light_pos,
            u_shadow_distance,

            u_is_transparent,
            u_diffuse_colour,
            u_has_texture_diffuse,
            u_texture_diffuse,

            num_shader_uniforms,
        };

    protected:
        void assign_uniforms() {
            // Vertex Shader
            uniform_handles_[uniform::u_texture_offset] = get_uniform_location("u_texture_offset");
            uniform_handles_[uniform::u_texture_scale] = get_uniform_location("u_texture_scale");

            // Geometry Shader
            uniform_handles_[uniform::u_view_projection_matrices0] = get_uniform_location("u_view_projection_matrices[0]");
            uniform_handles_[uniform::u_view_projection_matrices1] = get_uniform_location("u_view_projection_matrices[1]");
            uniform_handles_[uniform::u_view_projection_matrices2] = get_uniform_location("u_view_projection_matrices[2]");
            uniform_handles_[uniform::u_view_projection_matrices3] = get_uniform_location("u_view_projection_matrices[3]");
            uniform_handles_[uniform::u_view_projection_matrices4] = get_uniform_location("u_view_projection_matrices[4]");
            uniform_handles_[uniform::u_view_projection_matrices5] = get_uniform_location("u_view_projection_matrices[5]");

            // Fragment Shader
            uniform_handles_[uniform::u_light_pos] = get_uniform_location("u_light_pos");
            uniform_handles_[uniform::u_shadow_distance] = get_uniform_location("u_shadow_distance");

            uniform_handles_[uniform::u_is_transparent] = get_uniform_location("u_is_transparent");
            uniform_handles_[uniform::u_diffuse_colour] = get_uniform_location("u_diffuse_colour");
            uniform_handles_[uniform::u_has_texture_diffuse] = get_uniform_location("u_has_texture_diffuse");
            uniform_handles_[uniform::u_texture_diffuse] = get_uniform_location("u_texture_diffuse");
        }

        void assign_textures() {
            set_uniform(uniform::u_texture_diffuse, (int32_t) texture_unit::texture_diffuse);
        }

    public:
        shadow_cubemap_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _gs_sources, const std::vector<std::string>& _fs_sources)
            : shader_program(_name, _vs_sources, _gs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
            assign_textures();
        }

        ~shadow_cubemap_shader() override = default;
    };
}