#pragma once

#include <unordered_map>
#include <SDL2/SDL_image.h>
#include <common/singleton.h>
#include "graphics/mesh.h"
#include "graphics/texture.h"

namespace mkr {
    class asset_loader : public singleton<asset_loader> {
        friend class singleton<asset_loader>;

    private:
        std::unordered_map<std::string, std::shared_ptr<texture_2d>> texture_2ds_;
        std::unordered_map<std::string, std::shared_ptr<texture_cube>> texture_cubes_;
        std::unordered_map<std::string, std::shared_ptr<mesh>> meshes_;

        asset_loader() {}
        ~asset_loader() {}

    public:
        std::shared_ptr<texture_2d> get_texture_2d(const std::string& _name) {
            auto iter = texture_2ds_.find(_name);
            return (iter == texture_2ds_.end()) ? nullptr : iter->second;
        }

        std::shared_ptr<texture_cube> get_texture_cube(const std::string& _name) {
            auto iter = texture_cubes_.find(_name);
            return (iter == texture_cubes_.end()) ? nullptr : iter->second;
        }

        std::shared_ptr<texture_2d> load_texture_2d(const std::string& _name, const std::string& _file, bool _flip_x = false, bool _flip_y = true) {
            SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            SDL_Surface* raw_surface = IMG_Load(_file.c_str());
            if (raw_surface == nullptr) {
                spdlog::error("unable to load texture: {}", _file);
            }

            SDL_Surface* converted_surface = SDL_ConvertSurface(raw_surface, pixel_format, 0);
            if (converted_surface == nullptr) {
                spdlog::error("unable to convert texture: {}", _file);
            }

            int width = converted_surface->w;
            int height = converted_surface->h;
            void* pixelData = converted_surface->pixels;

            auto texture = std::make_shared<texture_2d>(_name, width, height, pixelData);
            texture_2ds_[_name] = texture;

            SDL_FreeFormat(pixel_format);
            SDL_FreeSurface(raw_surface);
            SDL_FreeSurface(converted_surface);

            return texture;
        }

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
    };
}