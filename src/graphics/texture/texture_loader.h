#pragma once

#include <cstring>
#include <memory>
#include <SDL2/SDL_image.h>
#include <log/log.h>
#include "graphics/texture/texture.h"

namespace mkr {
    class texture_loader {
    private:
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
                    uint8_t* left_pixel = &row[c *
                                               _bytes_per_pixel]; // Left Pixel (1st Pixel, 2nd Pixel, 3rd Pixel etc...)
                    uint8_t* right_pixel = &row[(_width - c - 1) *
                                                _bytes_per_pixel]; // Right Pixel (Last Pixel, 2nd Last Pixel, 3rd Last Pixel etc...)
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

    public:
        texture_loader() = delete;

        static std::unique_ptr<texture2d> load_texture2d(const std::string& _name, const std::string& _file, bool _flip_x, bool _flip_y) {
            SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            SDL_Surface* raw_surface = IMG_Load(_file.c_str());
            if (raw_surface == nullptr) {
                MKR_CORE_ERROR("unable to load texture: {}", _file);
            }

            SDL_Surface* converted_surface = SDL_ConvertSurface(raw_surface, pixel_format, 0);
            if (converted_surface == nullptr) {
                MKR_CORE_ERROR("unable to convert texture: {}", _file);
            }

            void* pixel_data = converted_surface->pixels;
            if (_flip_x) {
                flip_image_x(pixel_data, converted_surface->w, converted_surface->h, pixel_format->BytesPerPixel);
            }
            if (!_flip_y) { // SDL_Image fucking stupidly loads the image upside down, so we need to reverse it.
                flip_image_y(pixel_data, converted_surface->w, converted_surface->h, pixel_format->BytesPerPixel);
            }

            auto tex = std::make_unique<texture2d>(_name, converted_surface->w, converted_surface->h, pixel_data);

            SDL_FreeFormat(pixel_format);
            SDL_FreeSurface(raw_surface);
            SDL_FreeSurface(converted_surface);

            return tex;
        }

        static std::unique_ptr<cubemap> load_cubemap(const std::string& _name, std::array<std::string, num_cubemap_sides> _files, bool _flip_x, bool _flip_y) {
            SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            SDL_Surface* raw_surfaces[num_cubemap_sides] = {nullptr};
            SDL_Surface* converted_surfaces[num_cubemap_sides] = {nullptr};

            for (auto i = 0; i < num_cubemap_sides; ++i) {
                raw_surfaces[i] = IMG_Load(_files[i].c_str());
                if (raw_surfaces[i] == nullptr) {
                    MKR_CORE_ERROR("unable to load texture: {}", _files[i]);
                }

                converted_surfaces[i] = SDL_ConvertSurface(raw_surfaces[i], pixel_format, 0);
                if (converted_surfaces[i] == nullptr) {
                    MKR_CORE_ERROR("unable to convert texture: {}", _files[i]);
                }

                void* pixel_data = converted_surfaces[i]->pixels;
                if (_flip_x) {
                    flip_image_x(pixel_data, converted_surfaces[i]->w, converted_surfaces[i]->h, pixel_format->BytesPerPixel);
                }
                if (!_flip_y) {
                    flip_image_y(pixel_data, converted_surfaces[i]->w, converted_surfaces[i]->h, pixel_format->BytesPerPixel);
                }
            }

            if (converted_surfaces[0]->w != converted_surfaces[0]->h) {
                const std::string err_msg = "texture_loader::load_cubemap(): All 6 cube textures must be the same size, and every texture must be a square!";
                MKR_CORE_ERROR(err_msg);
                throw std::runtime_error(err_msg);
            }

            auto tex = std::make_unique<cubemap>(_name, converted_surfaces[0]->h,
                                                 std::array<const void*, num_cubemap_sides>{
                                                      converted_surfaces[0]->pixels,
                                                      converted_surfaces[1]->pixels,
                                                      converted_surfaces[2]->pixels,
                                                      converted_surfaces[3]->pixels,
                                                      converted_surfaces[4]->pixels,
                                                      converted_surfaces[5]->pixels
                                                  });

            SDL_FreeFormat(pixel_format);
            for (auto i = 0; i < num_cubemap_sides; ++i) {
                SDL_FreeSurface(raw_surfaces[i]);
                SDL_FreeSurface(converted_surfaces[i]);
            }

            return tex;
        }
    };
} // mkr