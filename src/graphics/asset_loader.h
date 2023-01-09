#pragma once

#include <unordered_map>
#include <common/singleton.h>
#include "graphics/shader_program.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"

namespace mkr {
    class asset_loader : public singleton<asset_loader> {
        friend class singleton<asset_loader>;

    private:
        std::unordered_map<std::string, std::shared_ptr<shader_program>> shader_programs_;
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
        // Shaders
        std::shared_ptr<shader_program> get_shader_program(const std::string& _name);

        std::shared_ptr<shader_program> load_shader_program(const std::string& _name, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _fs_files);

        // Texture 2D
        std::shared_ptr<texture_2d> get_texture_2d(const std::string& _name);

        std::shared_ptr<texture_2d> load_texture_2d(const std::string& _name, const std::string& _file, bool _flip_x = false, bool _flip_y = false);

        // Texture Cube
        std::shared_ptr<texture_cube> get_texture_cube(const std::string& _name);

        std::shared_ptr<texture_cube> load_texture_cube(const std::string& _name, std::array<std::string, num_texture_cube_sides> _files, bool _flip_x = false, bool _flip_y = false);

        // Mesh
        std::shared_ptr<mesh> get_mesh(const std::string& _name);

        std::shared_ptr<mesh> make_skybox(const std::string& _name);

        std::shared_ptr<mesh> make_screen_quad(const std::string& _name);

        std::shared_ptr<mesh> make_quad(const std::string& _name);

        std::shared_ptr<mesh> load_obj(const std::string& _name, const std::string& _file);
    };
}