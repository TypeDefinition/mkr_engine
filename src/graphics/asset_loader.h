#pragma once

#include <unordered_map>
#include <common/singleton.h>
#include <graphics/mesh.h>

// Include STD
#include <sstream>
#include <cstdio>

namespace mkr {
    class asset_loader : public singleton<asset_loader> {
        friend class singleton<asset_loader>;

    private:
        std::unordered_map<std::string, std::shared_ptr<mesh>> meshes_;

        asset_loader() {}
        ~asset_loader() {}

    public:
        std::shared_ptr<mesh> get_mesh(const std::string& _name) {
            auto iter = meshes_.find(_name);
            return (iter == meshes_.end()) ? nullptr : iter->second;
        }

        std::shared_ptr<mesh> make_triangle(const std::string &_name) {
            std::vector<vertex> vertices(3);

            vertices[0].position_ = {-0.5f, -0.5f, 0.0f};
            vertices[1].position_ = {0.5f, -0.5f, 0.0f};
            vertices[2].position_ = {0.0f, 0.5f, 0.0f};

            vertices[0].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[1].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[2].normal_ = {0.0f, 0.0f, 1.0f};

            vertices[0].tex_coord_ = {0.0f, 0.0f};
            vertices[1].tex_coord_ = {1.0f, 1.0f};
            vertices[2].tex_coord_ = {0.0f, 1.0f};

            vertices[0].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[1].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[2].tangent_ = {1.0f, 0.0f, 0.0f};

            std::vector<uint32_t> indices(3);
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;

            std::shared_ptr<mesh> mesh_ptr = std::make_shared<mesh>(_name, vertices, indices);
            meshes_.insert({_name, mesh_ptr});
            return mesh_ptr;
        }

        std::shared_ptr<mesh> make_screen_quad(const std::string &_name) {
            std::vector<vertex> vertices(4);

            vertices[0].position_ = {-1.0f, -1.0f, 0.0f};
            vertices[1].position_ = {1.0f, 1.0f, 0.0f};
            vertices[2].position_ = {-1.0f, 1.0f, 0.0f};
            vertices[3].position_ = {1.0f, -1.0f, 0.0f};

            vertices[0].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[1].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[2].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[3].normal_ = {0.0f, 0.0f, 1.0f};

            vertices[0].tex_coord_ = {0.0f, 0.0f};
            vertices[1].tex_coord_ = {1.0f, 1.0f};
            vertices[2].tex_coord_ = {0.0f, 1.0f};
            vertices[3].tex_coord_ = {1.0f, 0.0f};

            vertices[0].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[1].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[2].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[3].tangent_ = {1.0f, 0.0f, 0.0f};

            std::vector<uint32_t> indices(6);

            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;

            indices[3] = 0;
            indices[4] = 3;
            indices[5] = 1;

            std::shared_ptr<mesh> mesh_ptr = std::make_shared<mesh>(_name, vertices, indices);
            meshes_.insert({_name, mesh_ptr});
            return mesh_ptr;
        }

        std::shared_ptr<mesh> make_quad(const std::string &_name) {
            std::vector<vertex> vertices{4};

            vertices[0].position_ = {-0.5f, -0.5f, 0.0f};
            vertices[1].position_ = {0.5f, 0.5f, 0.0f};
            vertices[2].position_ = {-0.5f, 0.5f, 0.0f};
            vertices[3].position_ = {0.5f, -0.5f, 0.0f};

            vertices[0].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[1].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[2].normal_ = {0.0f, 0.0f, 1.0f};
            vertices[3].normal_ = {0.0f, 0.0f, 1.0f};

            vertices[0].tex_coord_ = {0.0f, 0.0f};
            vertices[1].tex_coord_ = {1.0f, 1.0f};
            vertices[2].tex_coord_ = {0.0f, 1.0f};
            vertices[3].tex_coord_ = {1.0f, 0.0f};

            vertices[0].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[1].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[2].tangent_ = {1.0f, 0.0f, 0.0f};
            vertices[3].tangent_ = {1.0f, 0.0f, 0.0f};

            std::vector<uint32_t> indices{6};

            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;

            indices[3] = 0;
            indices[4] = 3;
            indices[5] = 1;

            std::shared_ptr<mesh> mesh_ptr = std::make_shared<mesh>(_name, vertices, indices);
            meshes_.insert({_name, mesh_ptr});
            return mesh_ptr;
        }
    };
}