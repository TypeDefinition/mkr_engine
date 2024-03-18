#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class shadow_cubemap_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Vertex Shader
            u_view_projection_matrices0,
            u_view_projection_matrices1,
            u_view_projection_matrices2,
            u_view_projection_matrices3,
            u_view_projection_matrices4,
            u_view_projection_matrices5,

            u_light_pos,
            u_shadow_distance,

            num_shader_uniforms,
        };

    protected:
        void assign_uniforms() {
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
        }

    public:
        shadow_cubemap_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _gs_sources, const std::vector<std::string>& _fs_sources)
            : shader_program(_name, _vs_sources, _gs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
        }

        ~shadow_cubemap_shader() override = default;
    };
}