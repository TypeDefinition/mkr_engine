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
        mesh(const std::string& _name, const std::vector<vertex>& _vertices, const std::vector<uint32_t>& _indices)
                : name_{_name}, vertices_{_vertices}, indices_{_indices} {
            // VAO
            vao_ = std::make_shared<vao>();

            // EBO
            vao_->set_ebo(std::make_shared<ebo>(sizeof(uint32_t) * _indices.size(), (void*)_indices.data()));

            // VBO
            vbo_layout vertex_layout({
                                             vbo_element{vertex_attrib::position, GL_FLOAT, 3, sizeof(vector3), GL_FALSE},
                                             vbo_element{vertex_attrib::tex_coord, GL_FLOAT, 3, sizeof(vector3), GL_FALSE},
                                             vbo_element{vertex_attrib::normal, GL_FLOAT, 3, sizeof(vector3), GL_FALSE},
                                             vbo_element{vertex_attrib::tangent, GL_FLOAT, 3, sizeof(vector3), GL_FALSE}});
            std::shared_ptr<vbo> vertex_data = std::make_shared<vbo>(sizeof(vertex) * _vertices.size(), (void*)_vertices.data(), GL_STATIC_DRAW, vertex_layout, 0);
            vao_->set_vbo(vbo_index::vertex_data, vertex_data);

            // In OpenGL, a Vertex Attribute can only have a maximum of 4 Floats. So we need to break a matrix down into columns.
            vbo_layout instance_layout({
                                               vbo_element{vertex_attrib::model_matrix_col0, GL_FLOAT, 4, sizeof(float) * 4, GL_FALSE},
                                               vbo_element{vertex_attrib::model_matrix_col1, GL_FLOAT, 4, sizeof(float) * 4, GL_FALSE},
                                               vbo_element{vertex_attrib::model_matrix_col2, GL_FLOAT, 4, sizeof(float) * 4, GL_FALSE},
                                               vbo_element{vertex_attrib::model_matrix_col3, GL_FLOAT, 4, sizeof(float) * 4, GL_FALSE}});
            std::shared_ptr<vbo> instance_data = std::make_shared<vbo>(0, nullptr, GL_STREAM_DRAW, instance_layout, 1);
            vao_->set_vbo(vbo_index::instance_data, instance_data);
        }

        ~mesh() {}

        const std::string& name() const {
            return name_;
        }

        const std::vector<uint32_t>& indices() const {
            return indices_;
        }

        size_t num_indices() const {
            return indices_.size();
        }

        void bind() {
            vao_->bind();
        }

        void set_instance_data(const std::vector<mesh_instance>& _instances) {
            vao_->get_vbo(vbo_index::instance_data)->set_data(sizeof(mesh_instance) * _instances.size(), (void*)_instances.data(), GL_STREAM_DRAW);
        }
    };
}