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
    #define max_lights 8

    enum shader_uniform {
        u_view_projection_matrix,

        num_shader_uniforms,
    };

    enum class render_pass {
        skybox,
        geometry,
        lighting,
        forward,
        post_proc,
    };

    class shader_program {
    private:
        const std::string name_;
        const render_pass render_pass_;
        GLuint program_handle_;
        GLint uniform_handles_[num_shader_uniforms];

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
        void set_uniform(uint32_t _uniformID, float _value0);

        void set_uniform(uint32_t _uniformID, float _value0, float _value1);

        void set_uniform(uint32_t _uniformID, float _value0, float _value1, float _value2);

        void set_uniform(uint32_t _uniformID, float _value0, float _value1, float _value2, float _value3);

        // Integer
        void set_uniform(uint32_t _uniformID, int32_t _value0);

        void set_uniform(uint32_t _uniformID, int32_t _value0, int32_t _value1);

        void set_uniform(uint32_t _uniformID, int32_t _value0, int32_t _value1, int32_t _value2);

        void set_uniform(uint32_t _uniformID, int32_t _value0, int32_t _value1, int32_t _value2, int32_t _value3);

        // Unsigned Integer
        void set_uniform(uint32_t _uniformID, uint32_t _value0);

        void set_uniform(uint32_t _uniformID, uint32_t _value0, uint32_t _value1);

        void set_uniform(uint32_t _uniformID, uint32_t _value0, uint32_t _value1, uint32_t _value2);

        void set_uniform(uint32_t _uniformID, uint32_t _value0, uint32_t _value1, uint32_t _value2, uint32_t _value3);

        // Boolean
        void set_uniform(uint32_t _uniformID, bool _value0);

        void set_uniform(uint32_t _uniformID, bool _value0, bool _value1);

        void set_uniform(uint32_t _uniformID, bool _value0, bool _value1, bool _value2);

        void set_uniform(uint32_t _uniformID, bool _value0, bool _value1, bool _value2, bool _value3);

        // Matrix
        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix2x2& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix3x3& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix4x4& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix3x2& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix2x3& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix2x4& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix4x2& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix4x3& _value);

        void set_uniform(uint32_t _uniformID, bool _transpose, const matrix3x4& _value);

        // Colour
        void set_uniform(uint32_t _uniformID, const colour& _value);

        // Vector2
        void set_uniform(uint32_t _uniformID, const vector2& _value);

        // Vector3
        void set_uniform(uint32_t _uniformID, const vector3& _value);
    };
}