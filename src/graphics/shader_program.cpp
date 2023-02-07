#include <log/log.h>
#include "graphics/shader_program.h"

namespace mkr {
    GLuint shader_program::create_shader(GLenum _shader_type, const std::string& _shader_source) {
        // Create the shader.
        GLuint shader_handle = glCreateShader(_shader_type);
        // Set the source (the shader code) of the shader.
        const GLchar* source = _shader_source.c_str();
        glShaderSource(shader_handle, 1, &(source), nullptr);
        // Compile the shader.
        glCompileShader(shader_handle);

        // Verify compile status.
        GLint status = 0;
        glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint log_length = 0;
            glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);
            std::shared_ptr<GLchar> info_log{new GLchar[log_length]};
            glGetShaderInfoLog(shader_handle, log_length, &log_length, info_log.get());
            mkr::log::error(info_log.get());
            throw std::runtime_error(info_log.get());
        }

        return shader_handle;
    }

    GLint shader_program::get_uniform_location(const std::string& _uniform_name) const {
        GLint uniform_location = glGetUniformLocation(program_handle_, _uniform_name.c_str());
        if (uniform_location == -1) {
            std::string err_msg = "cannot find uniform " + _uniform_name + " in shader " + name_;
            mkr::log::warn(err_msg);
        }
        return uniform_location;
    }

    GLint shader_program::get_attrib_location(const std::string& _attrib_name) const {
        GLint attrib_location = glGetAttribLocation(program_handle_, _attrib_name.c_str());
        if (attrib_location == -1) {
            std::string err_msg = "cannot find attribute " + _attrib_name + " in shader " + name_;
            mkr::log::warn(err_msg);
        }
        return attrib_location;
    }

    void shader_program::assign_uniforms() {
        // Get uniform(s).
        // Assign textures to GL_TEXTURE0 to GL_TEXTUREN.
        // For some weird reason, glProgramUniform1i and glProgramUniform1iv are the only two functions
        // that may be used to load uniform variables defined as sampler types.
        // Loading samplers with any other function will result in a GL_INVALID_OPERATION error.
        // Thus, we must texture_unit to a signed integer.
        switch (render_pass_) {
            case render_pass::skybox:
                // Vertex Shader
                uniform_handles_[shader_uniform::u_view_projection_matrix] = get_uniform_location("u_view_projection_matrix");

                // Fragment Shader
                uniform_handles_[shader_uniform::u_sky_colour] = get_uniform_location("u_sky_colour");
                uniform_handles_[shader_uniform::u_texture_skybox_enabled] = get_uniform_location("u_texture_skybox_enabled");
                uniform_handles_[shader_uniform::u_texture_skybox] = get_uniform_location("u_texture_skybox");
                break;
            case render_pass::geometry:
                // Vertex Shader
                uniform_handles_[shader_uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
                uniform_handles_[shader_uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");
                uniform_handles_[shader_uniform::u_view_projection_matrix] = get_uniform_location("u_view_projection_matrix");

                // Fragment Shader
                uniform_handles_[shader_uniform::u_albedo_colour] = get_uniform_location("u_albedo_colour");
                uniform_handles_[shader_uniform::u_gloss] = get_uniform_location("u_gloss");
                uniform_handles_[shader_uniform::u_displacement_scale] = get_uniform_location("u_displacement_scale");

                uniform_handles_[shader_uniform::u_texture_albedo_enabled] = get_uniform_location("u_texture_albedo_enabled");
                uniform_handles_[shader_uniform::u_texture_normal_enabled] = get_uniform_location("u_texture_normal_enabled");
                uniform_handles_[shader_uniform::u_texture_specular_enabled] = get_uniform_location("u_texture_specular_enabled");
                uniform_handles_[shader_uniform::u_texture_gloss_enabled] = get_uniform_location("u_texture_gloss_enabled");
                uniform_handles_[shader_uniform::u_texture_displacement_enabled] = get_uniform_location("u_texture_displacement_enabled");

                uniform_handles_[shader_uniform::u_texture_albedo] = get_uniform_location("u_texture_albedo");
                uniform_handles_[shader_uniform::u_texture_normal] = get_uniform_location("u_texture_normal");
                uniform_handles_[shader_uniform::u_texture_specular] = get_uniform_location("u_texture_specular");
                uniform_handles_[shader_uniform::u_texture_gloss] = get_uniform_location("u_texture_gloss");
                uniform_handles_[shader_uniform::u_texture_displacement] = get_uniform_location("u_texture_displacement");
                break;
            case render_pass::lighting:
                // Fragment Shader
                uniform_handles_[shader_uniform::u_texture_gbuffer_position] = get_uniform_location("u_texture_gbuffer_position");
                uniform_handles_[shader_uniform::u_texture_gbuffer_normal] = get_uniform_location("u_texture_gbuffer_normal");
                uniform_handles_[shader_uniform::u_texture_gbuffer_albedo] = get_uniform_location("u_texture_gbuffer_albedo");
                uniform_handles_[shader_uniform::u_texture_gbuffer_specular] = get_uniform_location("u_texture_gbuffer_specular");
                uniform_handles_[shader_uniform::u_texture_gbuffer_gloss] = get_uniform_location("u_texture_gbuffer_gloss");

                uniform_handles_[shader_uniform::u_texture_lbuffer_diffuse] = get_uniform_location("u_texture_lbuffer_diffuse");
                uniform_handles_[shader_uniform::u_texture_lbuffer_specular] = get_uniform_location("u_texture_lbuffer_specular");

                uniform_handles_[shader_uniform::u_ambient_colour] = get_uniform_location("u_ambient_colour");
                uniform_handles_[shader_uniform::u_enable_lights] = get_uniform_location("u_enable_lights");
                uniform_handles_[shader_uniform::u_num_lights] = get_uniform_location("u_num_lights");
                for (auto i = 0; i < max_lights; ++i) {
                    uniform_handles_[i + shader_uniform::u_light_mode0] = get_uniform_location("u_lights[" + std::to_string(i) + "].mode_");
                    uniform_handles_[i + shader_uniform::u_light_power0] = get_uniform_location("u_lights[" + std::to_string(i) + "].power_");
                    uniform_handles_[i + shader_uniform::u_light_colour0] = get_uniform_location("u_lights[" + std::to_string(i) + "].colour_");
                    uniform_handles_[i + shader_uniform::u_light_attenuation_constant0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_constant_");
                    uniform_handles_[i + shader_uniform::u_light_attenuation_linear0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_linear_");
                    uniform_handles_[i + shader_uniform::u_light_attenuation_quadratic0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_quadratic_");
                    uniform_handles_[i + shader_uniform::u_light_spotlight_inner_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_inner_cosine_");
                    uniform_handles_[i + shader_uniform::u_light_spotlight_outer_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_outer_cosine_");
                    uniform_handles_[i + shader_uniform::u_light_position_camera_space0] = get_uniform_location("u_lights[" + std::to_string(i) + "].position_camera_space_");
                    uniform_handles_[i + shader_uniform::u_light_direction_camera_space0] = get_uniform_location("u_lights[" + std::to_string(i) + "].direction_camera_space_");
                }
                break;
            case render_pass::forward:
                // Vertex Shader
                uniform_handles_[shader_uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
                uniform_handles_[shader_uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");
                uniform_handles_[shader_uniform::u_view_projection_matrix] = get_uniform_location("u_view_projection_matrix");

                // Fragment Shader
                uniform_handles_[shader_uniform::u_ambient_colour] = get_uniform_location("u_ambient_colour");
                uniform_handles_[shader_uniform::u_albedo_colour] = get_uniform_location("u_albedo_colour");
                uniform_handles_[shader_uniform::u_gloss] = get_uniform_location("u_gloss");
                uniform_handles_[shader_uniform::u_displacement_scale] = get_uniform_location("u_displacement_scale");

                uniform_handles_[shader_uniform::u_texture_albedo_enabled] = get_uniform_location("u_texture_albedo_enabled");
                uniform_handles_[shader_uniform::u_texture_normal_enabled] = get_uniform_location("u_texture_normal_enabled");
                uniform_handles_[shader_uniform::u_texture_specular_enabled] = get_uniform_location("u_texture_specular_enabled");
                uniform_handles_[shader_uniform::u_texture_gloss_enabled] = get_uniform_location("u_texture_gloss_enabled");
                uniform_handles_[shader_uniform::u_texture_displacement_enabled] = get_uniform_location("u_texture_displacement_enabled");

                uniform_handles_[shader_uniform::u_texture_albedo] = get_uniform_location("u_texture_albedo");
                uniform_handles_[shader_uniform::u_texture_normal] = get_uniform_location("u_texture_normal");
                uniform_handles_[shader_uniform::u_texture_specular] = get_uniform_location("u_texture_specular");
                uniform_handles_[shader_uniform::u_texture_gloss] = get_uniform_location("u_texture_gloss");
                uniform_handles_[shader_uniform::u_texture_displacement] = get_uniform_location("u_texture_displacement");

                uniform_handles_[shader_uniform::u_enable_lights] = get_uniform_location("u_enable_lights");
                uniform_handles_[shader_uniform::u_num_lights] = get_uniform_location("u_num_lights");
                for (auto i = 0; i < max_lights; ++i) {
                    uniform_handles_[i + shader_uniform::u_light_mode0] = get_uniform_location("u_lights[" + std::to_string(i) + "].mode_");
                    uniform_handles_[i + shader_uniform::u_light_power0] = get_uniform_location("u_lights[" + std::to_string(i) + "].power_");
                    uniform_handles_[i + shader_uniform::u_light_colour0] = get_uniform_location("u_lights[" + std::to_string(i) + "].colour_");
                    uniform_handles_[i + shader_uniform::u_light_attenuation_constant0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_constant_");
                    uniform_handles_[i + shader_uniform::u_light_attenuation_linear0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_linear_");
                    uniform_handles_[i + shader_uniform::u_light_attenuation_quadratic0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_quadratic_");
                    uniform_handles_[i + shader_uniform::u_light_spotlight_inner_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_inner_cosine_");
                    uniform_handles_[i + shader_uniform::u_light_spotlight_outer_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_outer_cosine_");
                    uniform_handles_[i + shader_uniform::u_light_position_camera_space0] = get_uniform_location("u_lights[" + std::to_string(i) + "].position_camera_space_");
                    uniform_handles_[i + shader_uniform::u_light_direction_camera_space0] = get_uniform_location("u_lights[" + std::to_string(i) + "].direction_camera_space_");
                }
                break;
            case render_pass::post_proc:
                break;
        }
    }

    void shader_program::assign_textures() {
        // Assign textures to GL_TEXTURE0 to GL_TEXTUREN.
        // For some weird reason, glProgramUniform1i and glProgramUniform1iv are the only two functions
        // that may be used to load uniform variables defined as sampler types.
        // Loading samplers with any other function will result in a GL_INVALID_OPERATION error.
        // Thus, we must texture_unit to a signed integer.
        switch (render_pass_) {
            case render_pass::skybox:
                set_uniform(shader_uniform::u_texture_skybox, (int32_t)texture_unit::texture_skybox);
                break;
            case render_pass::geometry:
                set_uniform(shader_uniform::u_texture_albedo, (int32_t)texture_unit::texture_albedo);
                set_uniform(shader_uniform::u_texture_normal, (int32_t)texture_unit::texture_normal);
                set_uniform(shader_uniform::u_texture_specular, (int32_t)texture_unit::texture_specular);
                set_uniform(shader_uniform::u_texture_gloss, (int32_t)texture_unit::texture_gloss);
                set_uniform(shader_uniform::u_texture_displacement, (int32_t)texture_unit::texture_displacement);
                break;
            case render_pass::lighting:
                set_uniform(shader_uniform::u_texture_gbuffer_position, (int32_t)texture_unit::texture_gbuffer_position);
                set_uniform(shader_uniform::u_texture_gbuffer_normal, (int32_t)texture_unit::texture_gbuffer_normal);
                set_uniform(shader_uniform::u_texture_gbuffer_albedo, (int32_t)texture_unit::texture_gbuffer_albedo);
                set_uniform(shader_uniform::u_texture_gbuffer_specular, (int32_t)texture_unit::texture_gbuffer_specular);
                set_uniform(shader_uniform::u_texture_gbuffer_gloss, (int32_t)texture_unit::texture_gbuffer_gloss);

                set_uniform(shader_uniform::u_texture_lbuffer_diffuse, (int32_t)texture_unit::texture_lbuffer_diffuse);
                set_uniform(shader_uniform::u_texture_lbuffer_specular, (int32_t)texture_unit::texture_lbuffer_specular);
                break;
            case render_pass::forward:
                set_uniform(shader_uniform::u_texture_albedo, (int32_t)texture_unit::texture_albedo);
                set_uniform(shader_uniform::u_texture_normal, (int32_t)texture_unit::texture_normal);
                set_uniform(shader_uniform::u_texture_specular, (int32_t)texture_unit::texture_specular);
                set_uniform(shader_uniform::u_texture_gloss, (int32_t)texture_unit::texture_gloss);
                set_uniform(shader_uniform::u_texture_displacement, (int32_t)texture_unit::texture_displacement);
                break;
            case render_pass::post_proc:
                set_uniform(shader_uniform::u_texture_fbuffer_composite, (int32_t)texture_unit::texture_fbuffer_composite);
                set_uniform(shader_uniform::u_texture_fbuffer_position, (int32_t)texture_unit::texture_fbuffer_position);
                set_uniform(shader_uniform::u_texture_fbuffer_normal, (int32_t)texture_unit::texture_fbuffer_normal);

                set_uniform(shader_uniform::u_texture_pbuffer_composite, (int32_t)texture_unit::texture_pbuffer_composite);
                break;
        }
    }

    shader_program::shader_program(const std::string& _name, render_pass _render_pass, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
            : name_{_name}, render_pass_{_render_pass} {
        // Create the shader program.
        program_handle_ = glCreateProgram();

        // Create the shaders.
        std::vector<GLuint> vs_handles;
        for (const auto& _vs_source: _vs_sources) {
            // Create the fragment shader. OpenGL supports multiple vertex shaders per shader program.
            GLuint handle = create_shader(GL_VERTEX_SHADER, _vs_source);
            vs_handles.push_back(handle);
            // Attach the shader to the program.
            glAttachShader(program_handle_, handle);
        }

        std::vector<GLuint> fs_handles;
        for (const auto& _fs_source: _fs_sources) {
            // Create the fragment shader. OpenGL supports multiple fragment shaders per shader program.
            GLuint handle = create_shader(GL_FRAGMENT_SHADER, _fs_source);
            fs_handles.push_back(handle);
            // Attach the shader to the program.
            glAttachShader(program_handle_, handle);
        }

        // Link the shader program. Now that we have attached the shaders, this will use the attached shaders to create an executable that will run on the programmable vertex processor.
        glLinkProgram(program_handle_);

        /**
         * Now that we are done creating the shader program, we no longer need the shaders, and they can be deleted.
         * It is also possible to store the shaders to create other shader programs,
         * but there isn't a compelling reason to do so since we can just re-create them again if necessary. */
        for (GLuint vs_handle: vs_handles) {
            glDetachShader(program_handle_, vs_handle);
            glDeleteShader(vs_handle);
        }
        for (GLuint fs_handle: fs_handles) {
            glDetachShader(program_handle_, fs_handle);
            glDeleteShader(fs_handle);
        }

        // Verify link status.
        GLint status = 0;
        glGetProgramiv(program_handle_, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint log_length = 0;
            glGetProgramiv(program_handle_, GL_INFO_LOG_LENGTH, &log_length);
            std::shared_ptr<GLchar> info_log{new GLchar[log_length]};
            glGetProgramInfoLog(program_handle_, log_length, &log_length, info_log.get());
            mkr::log::error(info_log.get());
            throw std::runtime_error(info_log.get());
        }

        mkr::log::info("shader program {} created", _name.c_str());

        assign_uniforms();
        assign_textures();
    }

    shader_program::~shader_program() {
        glDeleteProgram(program_handle_);
    }

    void shader_program::use() {
        glUseProgram(program_handle_);
    }

    // Float
    void shader_program::set_uniform(const std::string& _uniform_name, float _value0) {
        glProgramUniform1f(program_handle_, get_uniform_location(_uniform_name), _value0);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, float _value0, float _value1) {
        glProgramUniform2f(program_handle_, get_uniform_location(_uniform_name), _value0, _value1);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, float _value0, float _value1, float _value2) {
        glProgramUniform3f(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, float _value0, float _value1, float _value2, float _value3) {
        glProgramUniform4f(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2, _value3);
    }

    // Integer
    void shader_program::set_uniform(const std::string& _uniform_name, int32_t _value0) {
        glProgramUniform1i(program_handle_, get_uniform_location(_uniform_name), _value0);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1) {
        glProgramUniform2i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1);
    }

    void
    shader_program::set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1, int32_t _value2) {
        glProgramUniform3i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2);
    }

    void
    shader_program::set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1, int32_t _value2, int32_t _value3) {
        glProgramUniform4i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2, _value3);
    }

    // Unsigned Integer
    void shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0) {
        glProgramUniform1ui(program_handle_, get_uniform_location(_uniform_name), _value0);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1) {
        glProgramUniform2ui(program_handle_, get_uniform_location(_uniform_name), _value0, _value1);
    }

    void
    shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1, uint32_t _value2) {
        glProgramUniform3ui(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2);
    }

    void
    shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1, uint32_t _value2, uint32_t _value3) {
        glProgramUniform4ui(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2, _value3);
    }

    // Boolean
    void shader_program::set_uniform(const std::string& _uniform_name, bool _value0) {
        glProgramUniform1i(program_handle_, get_uniform_location(_uniform_name), _value0);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _value0, bool _value1) {
        glProgramUniform2i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _value0, bool _value1, bool _value2) {
        glProgramUniform3i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _value0, bool _value1, bool _value2, bool _value3) {
        glProgramUniform4i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2, _value3);
    }

    // Matrix
    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix2x2& _value) {
        glProgramUniformMatrix2fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix3x3& _value) {
        glProgramUniformMatrix3fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix4x4& _value) {
        glProgramUniformMatrix4fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix2x3& _value) {
        glProgramUniformMatrix2x3fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix3x2& _value) {
        glProgramUniformMatrix3x2fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix4x2& _value) {
        glProgramUniformMatrix4x2fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix2x4& _value) {
        glProgramUniformMatrix2x4fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix3x4& _value) {
        glProgramUniformMatrix3x4fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, bool _transpose, const matrix4x3& _value) {
        glProgramUniformMatrix4x3fv(program_handle_, get_uniform_location(_uniform_name), 1, _transpose, _value[0]);
    }

    // Colour
    void shader_program::set_uniform(const std::string& _uniform_name, const colour& _colour) {
        glProgramUniform4f(program_handle_, get_uniform_location(_uniform_name), _colour.r_, _colour.g_, _colour.b_, _colour.a_);
    }

    // Vector2
    void shader_program::set_uniform(const std::string& _uniform_name, const vector2& _value) {
        glProgramUniform2f(program_handle_, get_uniform_location(_uniform_name), _value.x_, _value.y_);
    }

    // Vector3
    void shader_program::set_uniform(const std::string& _uniform_name, const vector3& _value) {
        glProgramUniform3f(program_handle_, get_uniform_location(_uniform_name), _value.x_, _value.y_, _value.z_);
    }

    // Float
    void shader_program::set_uniform(uint32_t _shader_uniform, float _value0) {
        glProgramUniform1f(program_handle_, uniform_handles_[_shader_uniform], _value0);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, float _value0, float _value1) {
        glProgramUniform2f(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, float _value0, float _value1, float _value2) {
        glProgramUniform3f(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, float _value0, float _value1, float _value2, float _value3) {
        glProgramUniform4f(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2, _value3);
    }

    // Integer
    void shader_program::set_uniform(uint32_t _shader_uniform, int32_t _value0) {
        glProgramUniform1i(program_handle_, uniform_handles_[_shader_uniform], _value0);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, int32_t _value0, int32_t _value1) {
        glProgramUniform2i(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, int32_t _value0, int32_t _value1, int32_t _value2) {
        glProgramUniform3i(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, int32_t _value0, int32_t _value1, int32_t _value2, int32_t _value3) {
        glProgramUniform4i(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2, _value3);
    }

    // Unsigned Integer
    void shader_program::set_uniform(uint32_t _shader_uniform, uint32_t _value0) {
        glProgramUniform1ui(program_handle_, uniform_handles_[_shader_uniform], _value0);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, uint32_t _value0, uint32_t _value1) {
        glProgramUniform2ui(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, uint32_t _value0, uint32_t _value1, uint32_t _value2) {
        glProgramUniform3ui(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, uint32_t _value0, uint32_t _value1, uint32_t _value2, uint32_t _value3) {
        glProgramUniform4ui(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2, _value3);
    }

    // Boolean
    void shader_program::set_uniform(uint32_t _shader_uniform, bool _value0) {
        glProgramUniform1i(program_handle_, uniform_handles_[_shader_uniform], _value0);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _value0, bool _value1) {
        glProgramUniform2i(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _value0, bool _value1, bool _value2) {
        glProgramUniform3i(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _value0, bool _value1, bool _value2, bool _value3) {
        glProgramUniform4i(program_handle_, uniform_handles_[_shader_uniform], _value0, _value1, _value2, _value3);
    }

    // Matrix
    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix2x2& _value) {
        glProgramUniformMatrix2fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix3x3& _value) {
        glProgramUniformMatrix3fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix4x4& _value) {
        glProgramUniformMatrix4fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix2x3& _value) {
        glProgramUniformMatrix2x3fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix3x2& _value) {
        glProgramUniformMatrix3x2fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix4x2& _value) {
        glProgramUniformMatrix4x2fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix2x4& _value) {
        glProgramUniformMatrix2x4fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix3x4& _value) {
        glProgramUniformMatrix3x4fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    void shader_program::set_uniform(uint32_t _shader_uniform, bool _transpose, const matrix4x3& _value) {
        glProgramUniformMatrix4x3fv(program_handle_, uniform_handles_[_shader_uniform], 1, _transpose, _value[0]);
    }

    // Colour
    void shader_program::set_uniform(uint32_t _shader_uniform, const colour& _colour) {
        glProgramUniform4f(program_handle_, uniform_handles_[_shader_uniform], _colour.r_, _colour.g_, _colour.b_, _colour.a_);
    }

    // Vector2
    void shader_program::set_uniform(uint32_t _shader_uniform, const vector2& _value) {
        glProgramUniform2f(program_handle_, uniform_handles_[_shader_uniform], _value.x_, _value.y_);
    }

    // Vector3
    void shader_program::set_uniform(uint32_t _shader_uniform, const vector3& _value) {
        glProgramUniform3f(program_handle_, uniform_handles_[_shader_uniform], _value.x_, _value.y_, _value.z_);
    }
}