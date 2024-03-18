#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class shadow_2d_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Vertex Shader
            u_view_matrix,
            u_projection_matrix,

            num_shader_uniforms,
        };

    protected:
        void assign_uniforms() {
            // Vertex Shader
            uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
            uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");
        }

    public:
        shadow_2d_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
            : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
        }

        ~shadow_2d_shader() override = default;
    };
}