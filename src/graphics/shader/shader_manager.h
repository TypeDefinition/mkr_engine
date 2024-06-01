#pragma once

#include <iostream>
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include <glsl_include.h>
#include "graphics/shader/shader_program.h"
#include "util/file_util.h"

namespace mkr {
    class shader_manager : public singleton<shader_manager> {
        friend class singleton<shader_manager>;

    private:
        std::unordered_map<std::string, std::unique_ptr<shader_program>> shaders_;

    public:
        shader_manager() {}

        virtual ~shader_manager() {}

        // Shaders
        shader_program* get_shader(const std::string& _name) {
            auto iter = shaders_.find(_name);
            return (iter == shaders_.end()) ? nullptr : iter->second.get();
        }

        template<typename T>
        shader_program* make_shader(const std::string& _name, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _fs_files) requires std::is_base_of_v<shader_program, T> {
            if (shaders_.contains(_name)) { throw std::runtime_error("duplicate shader name"); }

            glsl_include vs;
            for (const auto& filename : _vs_files) {
                auto name = std::filesystem::path(filename).filename().string();
                auto source = file_util::file_to_str(filename);
                vs.add(name, source);
            }

            glsl_include fs;
            for (const auto& filename : _fs_files) {
                auto name = std::filesystem::path(filename).filename().string();
                auto source = file_util::file_to_str(filename);
                fs.add(name, source);
            }

            std::vector<std::string> vs_merged = { vs.merge() };
            std::vector<std::string> fs_merged = { fs.merge() };

            shaders_[_name] = std::make_unique<T>(_name, vs_merged, fs_merged);
            return shaders_[_name].get();
        }

        template<typename T>
        shader_program* make_shader(const std::string& _name, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _gs_files, const std::vector<std::string>& _fs_files) requires std::is_base_of_v<shader_program, T> {
            if (shaders_.contains(_name)) { throw std::runtime_error("duplicate shader name"); }

            glsl_include vs;
            for (const auto& filename : _vs_files) {
                auto name = std::filesystem::path(filename).filename().string();
                auto source = file_util::file_to_str(filename);
                vs.add(name, source);
            }

            glsl_include gs;
            for (const auto& filename : _gs_files) {
                auto name = std::filesystem::path(filename).filename().string();
                auto source = file_util::file_to_str(filename);
                gs.add(name, source);
            }

            glsl_include fs;
            for (const auto& filename : _fs_files) {
                auto name = std::filesystem::path(filename).filename().string();
                auto source = file_util::file_to_str(filename);
                fs.add(name, source);
            }

            std::vector<std::string> vs_merged = { vs.merge() };
            std::vector<std::string> gs_merged = { gs.merge() };
            std::vector<std::string> fs_merged = { fs.merge() };

            shaders_[_name] = std::make_unique<T>(_name, vs_merged, gs_merged, fs_merged);
            return shaders_[_name].get();
        }
    };
}