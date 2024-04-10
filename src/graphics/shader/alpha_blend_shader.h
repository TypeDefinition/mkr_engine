#pragma once

#include "graphics/shader/shader_program.h"
#include "graphics/lighting/lighting.h"
#include "graphics/shader/texture_unit.h"

namespace mkr {
    class alpha_blend_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Transform
            u_view_matrix,
            u_projection_matrix,

            // Textures
            u_texture_accumulation,
            u_texture_revealage,

            num_shader_uniforms,
        };

    protected:
        /**
         * Assign uniforms to uniform_handles_.
         */
        void assign_uniforms() {
            uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
            uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");

            uniform_handles_[uniform::u_texture_accumulation] = get_uniform_location("u_texture_accumulation");
            uniform_handles_[uniform::u_texture_revealage] = get_uniform_location("u_texture_revealage");
        }

        /**
         * Assign textures to GL_TEXTURE0 to GL_TEXTUREN.
         * For some weird reason, glProgramUniform1i and glProgramUniform1iv are the only two functions
         * that may be used to load uniform variables defined as sampler types.
         * Loading samplers with any other function will result in a GL_INVALID_OPERATION error.
         * Thus, we must cast texture_unit to a signed integer.
         */
        void assign_textures() {
            set_uniform(uniform::u_texture_accumulation, texture_unit::texture_accumulation);
            set_uniform(uniform::u_texture_revealage, texture_unit::texture_revealage);
        }

    public:
        alpha_blend_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources) : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
            assign_textures();
        }

        virtual ~alpha_blend_shader() {}
    };
} // mkr