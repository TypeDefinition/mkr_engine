#pragma once

#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include "graphics/shader/shader_program.h"
#include "graphics/material/material.h"
#include "graphics/mesh/mesh.h"
#include "graphics/texture/texture.h"
#include "graphics/lighting/skybox.h"
#include "util/file_util.h"

namespace mkr {
    class asset_loader : public singleton<asset_loader> {
        friend class singleton<asset_loader>;

    private:
        std::unordered_map<std::string, std::unique_ptr<shader_program>> shader_programs_;
        std::unordered_map<std::string, std::unique_ptr<texture_2d>> texture_2ds_;
        std::unordered_map<std::string, std::unique_ptr<texture_cube>> texture_cubes_;
        std::unordered_map<std::string, std::unique_ptr<material>> materials_;
        std::unordered_map<std::string, std::unique_ptr<mesh>> meshes_;

        /**
            \brief Flip the image data horizontally.
            \param _data The image data to flip.
            \param _width The horizontal resolution of the image data.
            \param _height The vertical resolution of the image data.
            \param _bytes_per_pixel How many bytes per pixel.
            \warning This function assumes that the image data is row major.
        */
        static void flip_image_x(void* _data, uint32_t _width, uint32_t _height, uint32_t _bytes_per_pixel);

        /**
            \brief Flip the image data vertically.
            \param _data The image data to flip.
            \param _width The horizontal resolution of the image data.
            \param _height The vertical resolution of the image data.
            \param _bytes_per_pixel How many bytes per pixel.
            \warning This function assumes that the image data is row major.
        */
        static void flip_image_y(void* _data, uint32_t _width, uint32_t _height, uint32_t _bytes_per_pixel);

        asset_loader() {}

        virtual ~asset_loader() {}

    public:
        void init();

        void exit();

        // Shaders
        shader_program* get_shader_program(const std::string& _name);

        template<typename T>
        shader_program* load_shader_program(const std::string& _name, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _fs_files) requires std::is_base_of_v<shader_program, T> {
            std::vector<std::string> vs_sources;
            for (const auto& filename : _vs_files) {
                vs_sources.push_back(file_util::file_to_str(filename));
            }

            std::vector<std::string> fs_sources;
            for (const auto& filename : _fs_files) {
                fs_sources.push_back(file_util::file_to_str(filename));
            }

            shader_programs_[_name] = std::make_unique<T>(_name, vs_sources, fs_sources);
            return shader_programs_[_name].get();
        }

        // Texture 2D
        texture_2d* get_texture_2d(const std::string& _name);

        texture_2d* load_texture_2d(const std::string& _name, const std::string& _file, bool _flip_x = false, bool _flip_y = false);

        // Texture Cube
        texture_cube* get_texture_cube(const std::string& _name);

        texture_cube* load_texture_cube(const std::string& _name, std::array<std::string, num_texture_cube_sides> _files, bool _flip_x = false, bool _flip_y = false);

        // Materials
        material* get_material(const std::string& _name);

        material* make_material(const std::string& _name);

        // Meshes
        static std::unique_ptr<mesh> make_skybox();

        static std::unique_ptr<mesh> make_screen_quad();

        static std::unique_ptr<mesh> make_triangle(const std::string& _name);

        static std::unique_ptr<mesh> make_quad(const std::string& _name);

        mesh* load_obj(const std::string& _name, const std::string& _file);

        mesh* get_mesh(const std::string& _name);
    };
}