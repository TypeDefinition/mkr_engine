#pragma once

#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include "graphics/shader/shader_program.h"
#include "util/file_util.h"

namespace mkr {
    class shader_manager : public singleton<shader_manager> {
        friend class singleton<shader_manager>;

    private:
        std::unordered_map<std::string, std::unique_ptr<shader_program>> shaders_;

        shader_manager() {}

        virtual ~shader_manager() {}

    public:
        // Shaders
        shader_program* get_shader(const std::string& _name) {
            auto iter = shaders_.find(_name);
            return (iter == shaders_.end()) ? nullptr : iter->second.get();
        }

        template<typename T>
        shader_program* make_shader(const std::string& _name, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _fs_files) requires std::is_base_of_v<shader_program, T> {
            std::vector<std::string> vs_sources;
            for (const auto& filename : _vs_files) {
                vs_sources.push_back(file_util::file_to_str(filename));
            }

            std::vector<std::string> fs_sources;
            for (const auto& filename : _fs_files) {
                fs_sources.push_back(file_util::file_to_str(filename));
            }

            shaders_[_name] = std::make_unique<T>(_name, vs_sources, fs_sources);
            return shaders_[_name].get();
        }

        template<typename T>
        shader_program* make_shader(const std::string& _name, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _gs_files, const std::vector<std::string>& _fs_files) requires std::is_base_of_v<shader_program, T> {
            std::vector<std::string> vs_sources;
            for (const auto& filename : _vs_files) {
                vs_sources.push_back(file_util::file_to_str(filename));
            }

            std::vector<std::string> gs_sources;
            for (const auto& filename : _gs_files) {
                gs_sources.push_back(file_util::file_to_str(filename));
            }

            std::vector<std::string> fs_sources;
            for (const auto& filename : _fs_files) {
                fs_sources.push_back(file_util::file_to_str(filename));
            }

            shaders_[_name] = std::make_unique<T>(_name, vs_sources, gs_sources, fs_sources);
            return shaders_[_name].get();
        }
    };
}