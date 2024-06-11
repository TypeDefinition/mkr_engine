#pragma once

#include <cstring>
#include <stdexcept>
#include <memory>
#include <stb/stb_image.h>
#include <log/log.h>
#include "graphics/texture/texture.h"

namespace mkr {
    class texture_loader {
    public:
        texture_loader() = delete;

        static std::unique_ptr<texture2d> load_texture2d(const std::string& _name, const std::string& _file) {
            stbi_set_flip_vertically_on_load(true);

            // Standardise all textures to RGBA.
            int width, height, channels;
            stbi_uc* data = stbi_load(_file.c_str(), &width, &height, &channels, STBI_rgb_alpha);
            if (data == nullptr) {
                MKR_CORE_ERROR("unable to load texture: {}", _file);
                throw std::runtime_error("failed to load texture");
            }

            auto tex = std::make_unique<texture2d>(_name, width, height, data, sized_format::rgba8);
            stbi_image_free(data);
            return tex;
        }

        static std::unique_ptr<cubemap> load_cubemap(const std::string& _name, std::array<std::string, num_cubemap_sides> _files) {
            stbi_set_flip_vertically_on_load(false);

            int widths[num_cubemap_sides];
            int heights[num_cubemap_sides];
            int channels[num_cubemap_sides];
            stbi_uc* data[num_cubemap_sides] = {nullptr};
            for (auto i = 0; i < num_cubemap_sides; ++i) {
                // Standardise all textures to RGBA.
                data[i] = stbi_load(_files[i].c_str(), &widths[i], &heights[i], &channels[i], STBI_rgb_alpha);
                if (data[i] == nullptr) {
                    MKR_CORE_ERROR("unable to load texture: {}", _files[i]);
                    throw std::runtime_error("failed to load texture");
                }
                if (widths[i] != heights[i]) {
                    MKR_CORE_ERROR("cubemap textures must be a square: {}", _files[i]);
                    throw std::runtime_error("non-square cubemap texture");
                }
            }

            for (auto i = 1; i < num_cubemap_sides; ++i) {
                if (widths[i] != widths[i - 1]) {
                    MKR_CORE_ERROR("all textures in a cubemap must be the same size: {}", _files[i]);
                    throw std::runtime_error("inconsistent cubemap texture size");
                }
            }

            auto tex = std::make_unique<cubemap>(_name, widths[0], std::array<const void*, num_cubemap_sides>{data[0], data[1], data[2], data[3], data[4], data[5]});
            for (auto i = 0; i < num_cubemap_sides; ++i) {
                stbi_image_free(data[i]);
            }
            return tex;
        }
    };
} // mkr