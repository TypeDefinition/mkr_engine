#pragma once

#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include "graphics/texture/texture.h"
#include "graphics/texture/texture_loader.h"

namespace mkr {
    class texture_manager : public singleton<texture_manager> {
        friend class singleton<texture_manager>;

    private:
        std::unordered_map<std::string, std::unique_ptr<texture2d>> texture2ds_;
        std::unordered_map<std::string, std::unique_ptr<cubemap>> cubemaps_;

    public:
        texture_manager() {}

        virtual ~texture_manager() {}

        // Texture 2D
        texture2d* get_texture2d(const std::string& _name) {
            auto iter = texture2ds_.find(_name);
            return (iter == texture2ds_.end()) ? nullptr : iter->second.get();
        }

        texture2d* make_texture2d(const std::string& _name, const std::string& _file) {
            if (texture2ds_.contains(_name)) { throw std::runtime_error("duplicate texture2d name"); }
            texture2ds_[_name] = texture_loader::load_texture2d(_name, _file);
            return texture2ds_[_name].get();
        }

        // Texture Cube
        cubemap* get_cubemap(const std::string& _name) {
            auto iter = cubemaps_.find(_name);
            return (iter == cubemaps_.end()) ? nullptr : iter->second.get();
        }

        // Skybox textures need to be flipped on the Y-axis due to some stupid OpenGL cubemap convention.
        cubemap* make_cubemap(const std::string& _name, const std::array<std::string, num_cubemap_sides>& _files) {
            if (cubemaps_.contains(_name)) { throw std::runtime_error("duplicate cubemap name"); }
            cubemaps_[_name] = texture_loader::load_cubemap(_name, _files);
            return cubemaps_[_name].get();
        }
    };
}