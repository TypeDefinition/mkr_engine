#pragma once

#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include "graphics/texture/texture.h"
#include "graphics/texture/texture_loader.h"

namespace mkr {
    class texture_manager : public singleton<texture_manager> {
        friend class singleton<texture_manager>;

    private:
        std::unordered_map<std::string, std::unique_ptr<texture_2d>> texture_2ds_;
        std::unordered_map<std::string, std::unique_ptr<texture_cube>> texture_cubes_;

        texture_manager() {}

        virtual ~texture_manager() {}

    public:
        // Texture 2D
        texture_2d* get_texture_2d(const std::string& _name) {
            auto iter = texture_2ds_.find(_name);
            return (iter == texture_2ds_.end()) ? nullptr : iter->second.get();
        }

        texture_2d* make_texture_2d(const std::string& _name, const std::string& _file, bool _flip_x = false, bool _flip_y = false) {
            texture_2ds_[_name] = texture_loader::load_texture_2d(_name, _file, _flip_x, _flip_y);
            return texture_2ds_[_name].get();
        }

        // Texture Cube
        texture_cube* get_texture_cube(const std::string& _name) {
            auto iter = texture_cubes_.find(_name);
            return (iter == texture_cubes_.end()) ? nullptr : iter->second.get();
        }

        texture_cube* make_texture_cube(const std::string& _name, std::array<std::string, num_texture_cube_sides> _files, bool _flip_x = false, bool _flip_y = false) {
            texture_cubes_[_name] = texture_loader::load_texture_cube(_name, _files, _flip_x, _flip_y);
            return texture_cubes_[_name].get();
        }
    };
}