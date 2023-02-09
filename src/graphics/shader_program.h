#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <maths/colour.h>
#include <maths/vector2.h>
#include <maths/vector3.h>
#include <maths/matrix.h>
#include "graphics/texture.h"

namespace mkr {
    #define max_lights 32

    enum shader_uniform : uint32_t {
        // Vertex Shader
        u_view_matrix, // G-Pass, F-Pass
        u_projection_matrix, // G-Pass, F-Pass
        u_view_projection_matrix, // G-Pass, F-Pass, Skybox

        // Fragment Shader
        u_sky_colour, // Skybox
        u_ambient_colour, // L-Pass, F-Pass
        u_albedo_colour, // G-Pass, F-Pass
        u_gloss, // G-Pass, F-Pass
        u_displacement_scale, // G-Pass, F-Pass

        u_near, // P-Pass
        u_far, // P-Pass
        u_bottom_left, // P-Pass
        u_top_right, // P-Pass

        u_texture_skybox_enabled, // Skybox
        u_texture_skybox, // Skybox

        u_texture_albedo_enabled, // G-Pass, F-Pass
        u_texture_normal_enabled, // G-Pass, F-Pass
        u_texture_specular_enabled, // G-Pass, F-Pass
        u_texture_gloss_enabled, // G-Pass, F-Pass
        u_texture_displacement_enabled, // G-Pass, F-Pass
        u_texture_albedo, // G-Pass, F-Pass
        u_texture_normal, // G-Pass, F-Pass
        u_texture_specular, // G-Pass, F-Pass
        u_texture_gloss, // G-Pass, F-Pass
        u_texture_displacement, // G-Pass, F-Pass

        u_texture_frag_position, // L-Pass, P-Pass
        u_texture_frag_normal, // L-Pass, P-Pass
        u_texture_frag_albedo, // L-Pass
        u_texture_frag_specular, // L-Pass
        u_texture_frag_gloss, // L-Pass
        u_texture_light_diffuse, // L-Pass
        u_texture_light_specular, // L-Pass

        u_texture_composite, // P-Pass
        u_texture_depth_stencil, // P-Pass

        u_enable_lights,
        u_num_lights,
        u_light_mode0, // Light Mode 0 to N
        u_light_power0 = max_lights + u_light_mode0, // Light Power 0 to N
        u_light_colour0 = max_lights + u_light_power0, // Light Colour 0 to N
        u_light_attenuation_constant0 = max_lights + u_light_colour0, // Light Attenuation Constant 0 to N
        u_light_attenuation_linear0 = max_lights + u_light_attenuation_constant0, // Light Attenuation Linear 0 to N
        u_light_attenuation_quadratic0 = max_lights + u_light_attenuation_linear0, // Light Attenuation Quadratic 0 to N
        u_light_spotlight_inner_cosine0 = max_lights + u_light_attenuation_quadratic0, // Light Spotlight Inner Cosine 0 to N
        u_light_spotlight_outer_cosine0 = max_lights + u_light_spotlight_inner_cosine0, // Light Spotlight Outer Cosine 0 to N
        u_light_position_camera_space0 = max_lights + u_light_spotlight_outer_cosine0, // Light Position Camera Space 0 to N
        u_light_direction_camera_space0 = max_lights + u_light_position_camera_space0, // Light Direction Camera Space 0 to N

        num_shader_uniforms = max_lights + u_light_direction_camera_space0,
    };

    enum class render_pass {
        geometry,
        lighting,
        forward,
        skybox,
        post_proc,
    };

    class shader_program {
    private:
        const std::string name_;
        const render_pass render_pass_;
        GLuint program_handle_;
        GLint uniform_handles_[num_shader_uniforms] = { 0 };

        static GLuint create_shader(GLenum _shader_type, const std::string& _shader_source);

        GLint get_uniform_location(const std::string& _uniform_name) const;

        GLint get_attrib_location(const std::string& _attrib_name) const;

        void assign_uniforms();

        void assign_textures();

    public:
        shader_program(const std::string& _name, render_pass _render_pass, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources);

        virtual ~shader_program();

        inline const std::string& name() const { return name_; }

        void use();

        // Float
        void set_uniform(const std::string& _uniform_name, float _value0);

        void set_uniform(const std::string& _uniform_name, float _value0, float _value1);

        void set_uniform(const std::string& _uniform_name, float _value0, float _value1, float _value2);

        void set_uniform(const std::string& _uniform_name, float _value0, float _value1, float _value2, float _value3);

        // Integer
        void set_uniform(const std::string& _uniform_name, int32_t _value0);

        void set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1);

        void set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1, int32_t _value2);

        void set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1, int32_t _value2, int32_t _value3);

        // Unsigned Integer
        void set_uniform(const std::string& _uniform_name, uint32_t _value0);

        void set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1);

        void set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1, uint32_t _value2);

        void set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1, uint32_t _value2, uint32_t _value3);

        // Boolean
        void set_uniform(const std::string& _uniform_name, bool _value0);

        void set_uniform(const std::string& _uniform_name, bool _value0, bool _value1);

        void set_uniform(const std::string& _uniform_name, bool _value0, bool _value1, bool _value2);

        void set_uniform(const std::string& _uniform_name, bool _value0, bool _value1, bool _value2, bool _value3);

        // Matrix (Updated)
        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix2x2& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix3x3& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix4x4& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix3x2& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix2x3& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix2x4& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix4x2& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix4x3& _value);

        void set_uniform(const std::string& _uniform_name, bool _transpose, const matrix3x4& _value);

        // Colour
        void set_uniform(const std::string& _uniform_name, const colour& _value);

        // Vector2
        void set_uniform(const std::string& _uniform_name, const vector2& _value);

        // Vector3
        void set_uniform(const std::string& _uniform_name, const vector3& _value);

        // Float
        void set_uniform(uint32_t _shader_uniform, float _value0);

        void set_uniform(uint32_t _shader_uniform, float _value0, float _value1);

        void set_uniform(uint32_t _shader_uniform, float _value0, float _value1, float _value2);

        void set_uniform(uint32_t _shader_uniform, float _value0, float _value1, float _value2, float _value3);

        // Integer
        void set_uniform(uint32_t _shader_uniform, int32_t _value0);

        void set_uniform(uint32_t _shader_uniform, int32_t _value0, int32_t _value1);

        void set_uniform(uint32_t _shader_uniform, int32_t _value0, int32_t _value1, int32_t _value2);

        void set_uniform(uint32_t _shader_uniform, int32_t _value0, int32_t _value1, int32_t _value2, int32_t _value3);

        // Unsigned Integer
        void set_uniform(uint32_t _shader_uniform, uint32_t _value0);

        void set_uniform(uint32_t _shader_uniform, uint32_t _value0, uint32_t _value1);

        void set_uniform(uint32_t _shader_uniform, uint32_t _value0, uint32_t _value1, uint32_t _value2);

        void set_uniform(uint32_t _shader_uniform, uint32_t _value0, uint32_t _value1, uint32_t _value2, uint32_t _value3);

        // Boolean
        void set_uniform(uint32_t _shader_uniform, bool _value0);

        void set_uniform(uint32_t _shader_uniform, bool _value0, bool _value1);

        void set_uniform(uint32_t _shader_uniform, bool _value0, bool _value1, bool _value2);

        void set_uniform(uint32_t _shader_uniform, bool _value0, bool _value1, bool _value2, bool _value3);

        // Matrix
        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix2x2& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix3x3& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix4x4& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix3x2& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix2x3& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix2x4& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix4x2& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix4x3& _value);

        void set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix3x4& _value);

        // Colour
        void set_uniform(uint32_t _shader_uniform, const colour& _value);

        // Vector2
        void set_uniform(uint32_t _shader_uniform, const vector2& _value);

        // Vector3
        void set_uniform(uint32_t _shader_uniform, const vector3& _value);
    };
}