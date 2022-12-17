#pragma once

#include <GL/glew.h>

namespace mkr {
    class ebo {
    private:
        GLuint handle_;

    public:
        ebo(GLsizeiptr _size, void *_data) {
            glCreateBuffers(1, &handle_);
            glNamedBufferData(handle_, _size, _data, GL_STATIC_DRAW);
        }

        ~ebo() {
            glDeleteBuffers(1, &handle_);
        }

        GLuint handle() const {
            return handle_;
        }

        void bind() {
            // Must be bound only after VAO has been bound as this function modifies the VAO state. (https://www.khronos.org/opengl/wiki/Vertex_Specification, Vertex Array Object Section)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle_);
        }
    };
}