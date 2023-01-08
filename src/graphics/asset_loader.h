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

        /**
            \brief Flip the image data horizontally.
            \param _data The image data to flip.
            \param _width The horizontal resolution of the image data.
            \param _height The vertical resolution of the image data.
            \param _bytes_per_pixel How many bytes per pixel.
            \warning This function assumes that the image data is row major.
        */
        static void flip_image_x(void* _data, uint32_t _width, uint32_t _height, uint32_t _bytes_per_pixel) {
            const uint32_t bytes_per_row = _width * _bytes_per_pixel;
            uint8_t* temp_pixel = new uint8_t[_bytes_per_pixel];

            // For every row, we need to flip the first and last pixel.
            for (uint32_t r = 0; r < _height; ++r) {
                uint8_t* row = static_cast<uint8_t*>(_data) + (r * bytes_per_row);
                // Swap the first and last pixel, then the second and second last pixel, then the third and third last pixel...
                for (uint32_t c = 0; c < (_width >> 1); ++c) {
                    uint8_t* left_pixel = &row[c * _bytes_per_pixel]; // Left Pixel (1st Pixel, 2nd Pixel, 3rd Pixel etc...)
                    uint8_t* right_pixel = &row[(_width - c - 1) * _bytes_per_pixel]; // Right Pixel (Last Pixel, 2nd Last Pixel, 3rd Last Pixel etc...)
                    std::memcpy(temp_pixel, left_pixel, _bytes_per_pixel);
                    std::memcpy(left_pixel, right_pixel, _bytes_per_pixel);
                    std::memcpy(right_pixel, temp_pixel, _bytes_per_pixel);
                }
            }

            delete[] temp_pixel;
        }

        /**
            \brief Flip the image data vertically.
            \param _data The image data to flip.
            \param _width The horizontal resolution of the image data.
            \param _height The vertical resolution of the image data.
            \param _bytes_per_pixel How many bytes per pixel.
            \warning This function assumes that the image data is row major.
        */
        static void flip_image_y(void* _data, uint32_t _width, uint32_t _height, uint32_t _bytes_per_pixel) {
            const uint32_t bytes_per_row = _width * _bytes_per_pixel;
            uint8_t* temp_row = new uint8_t[bytes_per_row];

            // Since the data is stored in a row-major format, we can swap the data row by row rather than pixel by pixel.
            for (uint32_t r = 0; r < (_height >> 1); ++r) {
                uint8_t* top_row = &static_cast<uint8_t*>(_data)[r * bytes_per_row];
                uint8_t* bottom_row = &static_cast<uint8_t*>(_data)[(_height - r - 1) * bytes_per_row];
                std::memcpy(temp_row, top_row, bytes_per_row);
                std::memcpy(top_row, bottom_row, bytes_per_row);
                std::memcpy(bottom_row, temp_row, bytes_per_row);
            }

            delete[] temp_row;
        }

        asset_loader() {}

        virtual ~asset_loader() {}

    public:
        std::shared_ptr<texture_2d> get_texture_2d(const std::string& _name) {
            auto iter = texture_2ds_.find(_name);
            return (iter == texture_2ds_.end()) ? nullptr : iter->second;
        }

        std::shared_ptr<texture_cube> get_texture_cube(const std::string& _name) {
            auto iter = texture_cubes_.find(_name);
            return (iter == texture_cubes_.end()) ? nullptr : iter->second;
        }

        std::shared_ptr<texture_2d> load_texture_2d(const std::string& _name, const std::string& _file, bool _flip_x = false, bool _flip_y = false) {
            SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            SDL_Surface* raw_surface = IMG_Load(_file.c_str());
            if (raw_surface == nullptr) {
                mkr::log::error("unable to load texture: {}", _file);
            }

            SDL_Surface* converted_surface = SDL_ConvertSurface(raw_surface, pixel_format, 0);
            if (converted_surface == nullptr) {
                mkr::log::error("unable to convert texture: {}", _file);
            }

            int width = converted_surface->w;
            int height = converted_surface->h;
            void* pixel_data = converted_surface->pixels;

            if (_flip_x) {
                flip_image_x((uint8_t*)pixel_data, width, height, pixel_format->BytesPerPixel);
            }

            if (!_flip_y) { // SDL_Image fucking stupidly loads the image upside down, so we need to reverse it.
                flip_image_y(pixel_data, width, height, pixel_format->BytesPerPixel);
            }

            auto texture = std::make_shared<texture_2d>(_name, width, height, pixel_data);
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

        std::shared_ptr<mesh> make_screen_quad(const std::string& _name) {
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

        std::shared_ptr<mesh> make_quad(const std::string& _name) {
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