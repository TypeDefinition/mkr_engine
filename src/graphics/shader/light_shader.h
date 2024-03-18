#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class light_shader : public shader_program {
    public:
        static constexpr uint32_t max_lights = 8;

        enum uniform : uint32_t {
            u_texture_frag_position,
            u_texture_frag_normal,
            u_texture_frag_diffuse,
            u_texture_frag_specular,
            u_texture_frag_gloss,

            u_texture_light_diffuse,
            u_texture_light_specular,

            u_ambient_light,
            u_num_lights,
            u_light_mode0, // Light Mode 0 to N
            u_light_power0 = max_lights + u_light_mode0, // Light Power 0 to N
            u_light_colour0 = max_lights + u_light_power0, // Light Colour 0 to N
            u_light_attenuation_constant0 = max_lights + u_light_colour0, // Light Attenuation Constant 0 to N
            u_light_attenuation_linear0 = max_lights + u_light_attenuation_constant0, // Light Attenuation Linear 0 to N
            u_light_attenuation_quadratic0 = max_lights + u_light_attenuation_linear0, // Light Attenuation Quadratic 0 to N
            u_light_spotlight_inner_cosine0 = max_lights + u_light_attenuation_quadratic0, // Light Spotlight Inner Cosine 0 to N
            u_light_spotlight_outer_cosine0 = max_lights + u_light_spotlight_inner_cosine0, // Light Spotlight Outer Cosine 0 to N
            u_light_position0 = max_lights + u_light_spotlight_outer_cosine0, // Light Position Camera Space 0 to N
            u_light_direction0 = max_lights + u_light_position0, // Light Direction Camera Space 0 to N

            num_shader_uniforms = max_lights + u_light_direction0,
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
        light_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources);

        virtual ~light_shader() {}
    };
} // mkr