#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class geometry_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Vertex Shader
            u_view_matrix,
            u_projection_matrix,
            u_texture_offset,
            u_texture_scale,

            // Fragment Shader
            u_diffuse_colour,
            u_specular_colour,
            u_displacement_scale,

            u_has_texture_diffuse,
            u_has_texture_normal,
            u_has_texture_specular,
            u_has_texture_displacement,

            u_texture_diffuse,
            u_texture_normal,
            u_texture_specular,
            u_texture_displacement,

            num_shader_uniforms,
        };

    protected:
        /**
         * Assign uniforms to uniform_handles_.
         */
        void assign_uniforms();

        /**
         * Assign textures to GL_TEXTURE0 to GL_TEXTUREN.
         * For some weird reason, glProgramUniform1i and glProgramUniform1iv are the only two functions
         * that may be used to load uniform variables defined as sampler types.
         * Loading samplers with any other function will result in a GL_INVALID_OPERATION error.
         * Thus, we must cast texture_unit to a signed integer.
         */
        void assign_textures();

    public:
        geometry_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources);

        virtual ~geometry_shader() {}
    };
}