#pragma once

#include "graphics/shader/shader_program.h"
#include "graphics/lighting/lighting.h"

namespace mkr {
    class forward_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Transform
            u_view_matrix,
            u_projection_matrix,
            u_texture_offset,
            u_texture_scale,

            u_inv_view_matrix,

            // Material
            u_diffuse_colour,
            u_specular_colour,
            u_gloss,
            u_displacement_scale,

            // Textures
            u_texture_diffuse_enabled,
            u_texture_normal_enabled,
            u_texture_specular_enabled,
            u_texture_gloss_enabled,
            u_texture_displacement_enabled,

            u_texture_diffuse,
            u_texture_normal,
            u_texture_specular,
            u_texture_gloss,
            u_texture_displacement,

            // Shadows
            u_texture_shadows0,
            u_cubemap_shadows0 = lighting::max_lights + u_texture_shadows0,

            // Lights
            u_num_lights = lighting::max_lights + u_cubemap_shadows0,
            u_ambient_light,

            u_light_mode0, // Light Mode 0 to N
            u_light_power0 = lighting::max_lights + u_light_mode0, // Light Power 0 to N
            u_light_colour0 = lighting::max_lights + u_light_power0, // Light Colour 0 to N

            u_light_attenuation_constant0 = lighting::max_lights + u_light_colour0, // Light Attenuation Constant 0 to N
            u_light_attenuation_linear0 = lighting::max_lights + u_light_attenuation_constant0, // Light Attenuation Linear 0 to N
            u_light_attenuation_quadratic0 = lighting::max_lights + u_light_attenuation_linear0, // Light Attenuation Quadratic 0 to N

            u_light_spotlight_inner_cosine0 = lighting::max_lights + u_light_attenuation_quadratic0, // Spotlight Inner Cosine 0 to N
            u_light_spotlight_outer_cosine0 = lighting::max_lights + u_light_spotlight_inner_cosine0, // Spotlight Outer Cosine 0 to N

            u_light_position0 = lighting::max_lights + u_light_spotlight_outer_cosine0, // Light Position Camera Space 0 to N
            u_light_direction0 = lighting::max_lights + u_light_position0, // Light Direction Camera Space 0 to N

            u_light_shadow_distance0 = lighting::max_lights + u_light_direction0, // Light's shadow casting distance.
            u_light_view_projection_matrix0 = lighting::max_lights + u_light_shadow_distance0, // Spotlight/Directional Light View Projection Matrix

            num_shader_uniforms = lighting::max_lights + u_light_view_projection_matrix0,
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
        forward_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
            : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
            assign_textures();
        }

        virtual ~forward_shader() {}
    };
}