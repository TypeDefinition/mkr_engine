#pragma once

#include <vector>
#include <memory>
#include <GL/glew.h>

namespace mkr {
    /** A VAO can store up to 16 vertex attributes.
      * Vertex Attributes:
      *
      * Per Vertex Data (Stored in VBO 0):
      * 00 - Position
      * 01 - Normal
      * 02 - Texture Coordinate
      * 03 - Tangent

      * Per Instance Data (Stored in VBO 1):
      *
      * 04 - Instance Model View Matrix Column 0
      * 05 - Instance Model View Matrix Column 1
      * 06 - Instance Model View Matrix Column 2
      * 07 - Instance Model View Matrix Column 3
      * 08 - Instance Normal Matrix Column 0
      * 09 - Instance Normal Matrix Column 1
      * 10 - Instance Normal Matrix Column 2
      * 11 - To Be Defined
      * 12 - To Be Defined
      * 13 - To Be Defined
      * 14 - To Be Defined
      * 15 - To Be Defined */
    enum vertex_attrib : GLuint {
        position,
        normal,
        tex_coord,
        tangent,

        instance_model_view_mat_col0,
        instance_model_view_mat_col1,
        instance_model_view_mat_col2,
        instance_model_view_mat_col3,

        instance_normal_mat_col0,
        instance_normal_mat_col1,
        instance_normal_mat_col2,

        num_vertex_attrib,
    };

    enum vbo_index : GLuint {
        vertex_data = 0,
        instance_data,

        num_vbo,
    };

    struct vbo_element {
        const vertex_attrib attrib_;
        /// The type of data stored.
        const GLenum type_;
        /// The number of components.
        const GLint count_;
        /// Total size in bytes. byte_size_ = sizeof(type_) * count_
        const GLsizei byte_size_;
        /// Specifies whether fixed-point data values should be normalized (GL_TRUE) or converted
        /// directly as fixed-point values (GL_FALSE) when they are accessed.
        /// This parameter is ignored if type is GL_FIXED.
        const GLboolean normalised_;
    };

    class vbo_layout {
    private:
        std::vector<vbo_element> elements_;
        GLsizei byte_size_;

    public:
        vbo_layout(const std::vector<vbo_element> &_elements)
                : elements_{_elements}, byte_size_{0} {
            for (const auto &e: elements_) {
                byte_size_ += e.byte_size_;
            }
        }

        size_t num_elements() const {
            return elements_.size();
        }

        GLsizei byte_size() const {
            return byte_size_;
        }

        const vbo_element &operator[](size_t _index) const {
            return elements_[_index];
        }
    };

    class vbo {
    private:
        GLuint handle_;
        GLuint divisor_;
        vbo_layout layout_;

    public:
        vbo(GLsizeiptr _size, void *_data, GLenum _usage, vbo_layout _layout)
                : layout_{_layout} {
            glCreateBuffers(1, &handle_);
            glNamedBufferData(handle_, _size, _data, _usage);
        }

        ~vbo() {
            glDeleteBuffers(1, &handle_);
        }

        GLuint handle() const {
            return handle_;
        }

        GLuint divisor() const {
            return divisor_;
        }

        const vbo_layout &layout() const {
            return layout_;
        }

        void bind() {
            glBindBuffer(GL_ARRAY_BUFFER, handle_);
        }

        void set_data(GLsizeiptr _size, void *_data, GLenum _usage) {
            glNamedBufferData(handle_, _size, _data, _usage);
        }

        void set_sub_data(GLintptr _offset, GLsizeiptr _size, void *_data) {
            glNamedBufferSubData(handle_, _offset, _size, _data);
        }
    };
}