#pragma once

#include <memory>
#include <vector>
#include <string>
#include "graphics/vertex.h"
#include "graphics/vao.h"

namespace mkr {
    class mesh {
    private:
        const std::string name_;
        std::shared_ptr<vao> vao_;
        const std::vector<vertex> vertices_;
        const std::vector<uint32_t> indices_;

    public:
        mesh(const std::string &_name, const std::vector<vertex> &_vertices, const std::vector<uint32_t> &_indices)
                : name_{_name}, vertices_{_vertices}, indices_{_indices} {
            // VAO
            vao_ = std::make_shared<vao>();

            // VBO
            vbo_layout vertex_data_layout({
                                                  vbo_element{vertex_attrib::position, GL_FLOAT, 3, 3 * sizeof(float), GL_FALSE},
                                                  vbo_element{vertex_attrib::normal, GL_FLOAT, 3, 3 * sizeof(float), GL_FALSE},
                                                  vbo_element{vertex_attrib::tangent, GL_FLOAT, 3, 3 * sizeof(float), GL_FALSE},
                                                  vbo_element{vertex_attrib::tex_coord, GL_FLOAT, 2, 2 * sizeof(float), GL_FALSE}
                                          });
            std::shared_ptr<vbo> vertex_data = std::make_shared<vbo>(sizeof(vertex) * _vertices.size(), (void*)_vertices.data(), GL_STATIC_DRAW, vertex_data_layout);
            vao_->set_vbo(vbo_index::vertex_data, vertex_data);

            // EBO
            vao_->set_ebo(std::make_shared<ebo>(sizeof(uint32_t) * _indices.size(), (void*)_indices.data()));
        }

        ~mesh() {}

        const std::string& name() const {
            return name_;
        }

        void bind() {
            vao_->bind();
        }

        const std::vector<uint32_t>& indices() const {
            return indices_;
        }

        size_t num_indices() const {
            return indices_.size();
        }
    };
}