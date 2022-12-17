#pragma once

#include <GL/glew.h>

namespace mkr {
    enum texture_wrap_mode
    {
        repeat,
        mirrored_repeat,
        clamp_to_edge,
    };

    enum texture_shape
    {
        shape_2d,
        shape_cube,
    };

    class texture {
    private:
        GLuint handle_;
        texture_wrap_mode wrap_mode_ = texture_wrap_mode::repeat;
        texture_shape shape_ = texture_shape::shape_2d;

    public:
        texture() {}
        ~texture() {}

        void bind(GLuint _texture_unit) {
            glBindTextureUnit(_texture_unit, handle_);
        }
    };
}