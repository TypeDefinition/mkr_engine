#pragma once

#include <utility>
#include <vector>
#include "graphics/vbo.h"
#include "graphics/ebo.h"

namespace mkr {
    class vao {
    private:
        GLuint handle_;
        std::shared_ptr<vbo> vbos_[vbo_index::num_vbo];
        std::shared_ptr<ebo> ebo_;

    public:
        vao() {
            glCreateVertexArrays(1, &handle_);
        }

        ~vao() {
            glDeleteVertexArrays(1, &handle_);
        }

        void bind() {
            glBindVertexArray(handle_);
        }

        void set_vbo(vbo_index _type, std::shared_ptr<vbo> _vbo) {
            const vbo_layout& layout = _vbo->layout();
            glVertexArrayVertexBuffer(handle_, _type, _vbo->handle(), 0, layout.byte_size());
            glVertexArrayBindingDivisor(handle_, _type, _vbo->divisor());

            GLuint offset = 0;
            for (size_t i = 0; i < layout.num_elements(); ++i) {
                const vbo_element& e = layout[i];
                switch (e.type_) {
                    case GL_INT:
                    case GL_UNSIGNED_INT:
                        glVertexArrayAttribIFormat(handle_, e.attrib_, e.count_, e.type_, offset);
                        break;
                    case GL_FLOAT:
                        glVertexArrayAttribFormat(handle_, e.attrib_, e.count_, e.type_, e.normalised_, offset);
                        break;
                    case GL_DOUBLE:
                        glVertexArrayAttribLFormat(handle_, e.attrib_, e.count_, e.type_, offset);
                        break;
                    default:
                        throw std::runtime_error("invalid vbo element type");
                }
                glVertexArrayAttribBinding(handle_, e.attrib_, _type);
                glEnableVertexArrayAttrib(handle_, e.attrib_);
                offset += e.byte_size_;
            }

            vbos_[_type] = std::move(_vbo);
        }

        void set_ebo(std::shared_ptr<ebo> _ebo) {
            ebo_ = std::move(_ebo);
            glVertexArrayElementBuffer(handle_, ebo_->handle());
        }
    };
}