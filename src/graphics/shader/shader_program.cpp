#include <log/log.h>
#include "graphics/shader/shader_program.h"

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
            std::unique_ptr<GLchar> info_log{new GLchar[log_length]};
            glGetShaderInfoLog(shader_handle, log_length, &log_length, info_log.get());
            MKR_CORE_ERROR(info_log.get());
            throw std::runtime_error(info_log.get());
        }

        return shader_handle;
    }

    GLint shader_program::get_uniform_location(const std::string& _uniform_name) const {
        GLint uniform_location = glGetUniformLocation(program_handle_, _uniform_name.c_str());
        if (uniform_location == -1) {
            std::string err_msg = "cannot find uniform " + _uniform_name + " in shader " + name_;
            MKR_CORE_WARN(err_msg);
        }
        return uniform_location;
    }

    GLint shader_program::get_attrib_location(const std::string& _attrib_name) const {
        GLint attrib_location = glGetAttribLocation(program_handle_, _attrib_name.c_str());
        if (attrib_location == -1) {
            std::string err_msg = "cannot find attribute " + _attrib_name + " in shader " + name_;
            MKR_CORE_WARN(err_msg);
        }
        return attrib_location;
    }

    shader_program::shader_program(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources, size_t _num_uniforms)
        : name_{_name} {
        MKR_CORE_INFO("creating shader program {}", _name.c_str());

        // Allocate uniform handle array.
        uniform_handles_ = std::make_unique<GLint[]>(_num_uniforms);

        // Create the shader program.
        program_handle_ = glCreateProgram();

        // Create the shaders.
        std::vector<GLuint> vs_handles;
        for (const auto& src : _vs_sources) {
            // Create the fragment shader. OpenGL supports multiple vertex shaders per shader program.
            GLuint handle = create_shader(GL_VERTEX_SHADER, src);
            vs_handles.push_back(handle);
            // Attach the shader to the program.
            glAttachShader(program_handle_, handle);
        }

        std::vector<GLuint> fs_handles;
        for (const auto& src : _fs_sources) {
            // Create the fragment shader. OpenGL supports multiple fragment shaders per shader program.
            GLuint handle = create_shader(GL_FRAGMENT_SHADER, src);
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
        for (GLuint handle : vs_handles) {
            glDetachShader(program_handle_, handle);
            glDeleteShader(handle);
        }
        for (GLuint handle : fs_handles) {
            glDetachShader(program_handle_, handle);
            glDeleteShader(handle);
        }

        // Verify link status.
        GLint status = 0;
        glGetProgramiv(program_handle_, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint log_length = 0;
            glGetProgramiv(program_handle_, GL_INFO_LOG_LENGTH, &log_length);
            std::unique_ptr<GLchar> info_log{new GLchar[log_length]};
            glGetProgramInfoLog(program_handle_, log_length, &log_length, info_log.get());
            MKR_CORE_ERROR(info_log.get());
            throw std::runtime_error(info_log.get());
        }

        MKR_CORE_INFO("shader program {} created", _name.c_str());
    }

    shader_program::shader_program(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _gs_sources, const std::vector<std::string>& _fs_sources, size_t _num_uniforms) {
        MKR_CORE_INFO("creating shader program {}", _name.c_str());

        // Allocate uniform handle array.
        uniform_handles_ = std::make_unique<GLint[]>(_num_uniforms);

        // Create the shader program.
        program_handle_ = glCreateProgram();

        // Create the shaders.
        std::vector<GLuint> vs_handles;
        for (const auto& src : _vs_sources) {
            // Create the fragment shader. OpenGL supports multiple vertex shaders per shader program.
            GLuint handle = create_shader(GL_VERTEX_SHADER, src);
            vs_handles.push_back(handle);
            // Attach the shader to the program.
            glAttachShader(program_handle_, handle);
        }

        std::vector<GLuint> gs_handles;
        for (const auto& src : _gs_sources) {
            // Create the fragment shader. OpenGL supports multiple vertex shaders per shader program.
            GLuint handle = create_shader(GL_GEOMETRY_SHADER, src);
            gs_handles.push_back(handle);
            // Attach the shader to the program.
            glAttachShader(program_handle_, handle);
        }

        std::vector<GLuint> fs_handles;
        for (const auto& src : _fs_sources) {
            // Create the fragment shader. OpenGL supports multiple fragment shaders per shader program.
            GLuint handle = create_shader(GL_FRAGMENT_SHADER, src);
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
        for (GLuint handle : vs_handles) {
            glDetachShader(program_handle_, handle);
            glDeleteShader(handle);
        }
        for (GLuint handle : gs_handles) {
            glDetachShader(program_handle_, handle);
            glDeleteShader(handle);
        }
        for (GLuint handle : fs_handles) {
            glDetachShader(program_handle_, handle);
            glDeleteShader(handle);
        }

        // Verify link status.
        GLint status = 0;
        glGetProgramiv(program_handle_, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint log_length = 0;
            glGetProgramiv(program_handle_, GL_INFO_LOG_LENGTH, &log_length);
            std::unique_ptr<GLchar> info_log{new GLchar[log_length]};
            glGetProgramInfoLog(program_handle_, log_length, &log_length, info_log.get());
            MKR_CORE_ERROR(info_log.get());
            throw std::runtime_error(info_log.get());
        }

        MKR_CORE_INFO("shader program {} created", _name.c_str());
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

    void shader_program::set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1, int32_t _value2) {
        glProgramUniform3i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, int32_t _value0, int32_t _value1, int32_t _value2, int32_t _value3) {
        glProgramUniform4i(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2, _value3);
    }

    // Unsigned Integer
    void shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0) {
        glProgramUniform1ui(program_handle_, get_uniform_location(_uniform_name), _value0);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1) {
        glProgramUniform2ui(program_handle_, get_uniform_location(_uniform_name), _value0, _value1);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1, uint32_t _value2) {
        glProgramUniform3ui(program_handle_, get_uniform_location(_uniform_name), _value0, _value1, _value2);
    }

    void shader_program::set_uniform(const std::string& _uniform_name, uint32_t _value0, uint32_t _value1, uint32_t _value2, uint32_t _value3) {
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