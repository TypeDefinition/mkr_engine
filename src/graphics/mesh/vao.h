#pragma once

#include <utility>
#include <vector>
#include "graphics/mesh/vbo.h"
#include "graphics/mesh/ebo.h"

namespace mkr {
    class vao {
    private:
        GLuint handle_;
        std::unique_ptr<ebo> ebo_;
        std::unique_ptr<vbo> vbos_[vbo_index::num_vbo];

    public:
        vao() { glCreateVertexArrays(1, &handle_); }

        ~vao() { glDeleteVertexArrays(1, &handle_); }

        inline void bind() { glBindVertexArray(handle_); }

        [[nodiscard]] inline const ebo* get_ebo() const { return ebo_.get(); }

        [[nodiscard]] inline ebo* get_ebo() { return ebo_.get(); }

        void set_ebo(std::unique_ptr<ebo> _ebo) {
            ebo_ = std::move(_ebo);
            glVertexArrayElementBuffer(handle_, ebo_->handle());
        }

        [[nodiscard]] inline const vbo* get_vbo(vbo_index _type) const { return vbos_[_type].get(); }

        [[nodiscard]] inline vbo* get_vbo(vbo_index _type) { return vbos_[_type].get(); }

        void set_vbo(vbo_index _type, std::unique_ptr<vbo> _vbo) {
            const vbo_layout& layout = _vbo->layout();
            glVertexArrayVertexBuffer(handle_, _type, _vbo->handle(), 0, layout.bytes());
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
                offset += e.bytes_;
            }

            vbos_[_type] = std::move(_vbo);
        }
    };
}