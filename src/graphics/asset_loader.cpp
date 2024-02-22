#include <SDL2/SDL_image.h>
#include <maths/matrix_util.h>
#include "util/file_util.h"
#include <log/log.h>
#include "graphics/asset_loader.h"

namespace mkr {
    void asset_loader::flip_image_x(void* _data, uint32_t _width, uint32_t _height, uint32_t _bytes_per_pixel) {
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

    void asset_loader::flip_image_y(void* _data, uint32_t _width, uint32_t _height, uint32_t _bytes_per_pixel) {
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

    // Shaders
    shader_program* asset_loader::get_shader_program(const std::string& _name) {
        auto iter = shader_programs_.find(_name);
        return (iter == shader_programs_.end()) ? nullptr : iter->second.get();
    }

    shader_program* asset_loader::load_shader_program(const std::string& _name, render_pass _render_pass, const std::vector<std::string>& _vs_files, const std::vector<std::string>& _fs_files) {
        std::vector<std::string> vs_sources;
        for (const auto& filename: _vs_files) {
            vs_sources.push_back(file_util::file_to_str(filename));
        }

        std::vector<std::string> fs_sources;
        for (const auto& filename: _fs_files) {
            fs_sources.push_back(file_util::file_to_str(filename));
        }

        shader_programs_[_name] = std::make_unique<shader_program>(_name, _render_pass, vs_sources, fs_sources);
        return shader_programs_[_name].get();
    }

    // Texture 2D
    texture_2d* asset_loader::get_texture_2d(const std::string& _name) {
        auto iter = texture_2ds_.find(_name);
        return (iter == texture_2ds_.end()) ? nullptr : iter->second.get();
    }

    texture_2d* asset_loader::load_texture_2d(const std::string& _name, const std::string& _file, bool _flip_x, bool _flip_y) {
        SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
        SDL_Surface* raw_surface = IMG_Load(_file.c_str());
        if (raw_surface == nullptr) {
            mkr::log::error("unable to load texture: {}", _file);
        }

        SDL_Surface* converted_surface = SDL_ConvertSurface(raw_surface, pixel_format, 0);
        if (converted_surface == nullptr) {
            mkr::log::error("unable to convert texture: {}", _file);
        }

        void* pixel_data = converted_surface->pixels;
        if (_flip_x) {
            flip_image_x(pixel_data, converted_surface->w, converted_surface->h, pixel_format->BytesPerPixel);
        }
        if (!_flip_y) { // SDL_Image fucking stupidly loads the image upside down, so we need to reverse it.
            flip_image_y(pixel_data, converted_surface->w, converted_surface->h, pixel_format->BytesPerPixel);
        }

        texture_2ds_[_name] = std::make_unique<texture_2d>(_name, converted_surface->w, converted_surface->h, pixel_data);

        SDL_FreeFormat(pixel_format);
        SDL_FreeSurface(raw_surface);
        SDL_FreeSurface(converted_surface);

        return texture_2ds_[_name].get();
    }

    // Texture Cube
    texture_cube* asset_loader::get_texture_cube(const std::string& _name) {
        auto iter = texture_cubes_.find(_name);
        return (iter == texture_cubes_.end()) ? nullptr : iter->second.get();
    }

    texture_cube* asset_loader::load_texture_cube(const std::string& _name, std::array<std::string, num_texture_cube_sides> _files, bool _flip_x, bool _flip_y) {
        SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
        SDL_Surface* raw_surfaces[num_texture_cube_sides] = {nullptr};
        SDL_Surface* converted_surfaces[num_texture_cube_sides] = {nullptr};

        for (auto i = 0; i < num_texture_cube_sides; ++i) {
            raw_surfaces[i] = IMG_Load(_files[i].c_str());
            if (raw_surfaces[i] == nullptr) {
                mkr::log::error("unable to load texture: {}", _files[i]);
            }

            converted_surfaces[i] = SDL_ConvertSurface(raw_surfaces[i], pixel_format, 0);
            if (converted_surfaces[i] == nullptr) {
                mkr::log::error("unable to convert texture: {}", _files[i]);
            }

            void* pixel_data = converted_surfaces[i]->pixels;
            if (_flip_x) {
                flip_image_x(pixel_data, converted_surfaces[i]->w, converted_surfaces[i]->h, pixel_format->BytesPerPixel);
            }
            if (!_flip_y) {
                flip_image_y(pixel_data, converted_surfaces[i]->w, converted_surfaces[i]->h, pixel_format->BytesPerPixel);
            }
        }

        texture_cubes_[_name] = std::make_unique<texture_cube>(_name, converted_surfaces[0]->w, converted_surfaces[0]->h,
                                                                               std::array<const void*, num_texture_cube_sides>{
                                                                                       converted_surfaces[0]->pixels,
                                                                                       converted_surfaces[1]->pixels,
                                                                                       converted_surfaces[2]->pixels,
                                                                                       converted_surfaces[3]->pixels,
                                                                                       converted_surfaces[4]->pixels,
                                                                                       converted_surfaces[5]->pixels
                                                                               });

        SDL_FreeFormat(pixel_format);
        for (auto i = 0; i < num_texture_cube_sides; ++i) {
            SDL_FreeSurface(raw_surfaces[i]);
            SDL_FreeSurface(converted_surfaces[i]);
        }

        return texture_cubes_[_name].get();
    }

    // Materials
    material* asset_loader::get_material(const std::string& _name) {
        auto iter = materials_.find(_name);
        return (iter == materials_.end()) ? nullptr : iter->second.get();
    }

    material* asset_loader::make_material(const std::string& _name) {
        materials_[_name] = std::make_unique<material>();;
        return materials_[_name].get();
    }

    // Meshes
    std::unique_ptr<mesh> asset_loader::make_skybox() {
        std::vector<vertex> vertices(24);
        std::vector<uint32_t> indices;

        // Front
        vertices[0].position_ = {1.0f, -1.0f, 1.0f}; // Bottom Left
        vertices[1].position_ = {-1.0f, 1.0f, 1.0f}; // Top Right
        vertices[2].position_ = {1.0f, 1.0f, 1.0f}; // Top Left
        vertices[3].position_ = {-1.0f, -1.0f, 1.0f}; // Bottom Right

        vertices[0].tex_coord_ = {-1.0f, 1.0f, 1.0f};
        vertices[1].tex_coord_ = {1.0f, -1.0f, 1.0f};
        vertices[2].tex_coord_ = {-1.0f, -1.0f, 1.0f};
        vertices[3].tex_coord_ = {1.0f, 1.0f, 1.0f};

        vertices[0].normal_ = {0.0f, 0.0f, -1.0f};
        vertices[1].normal_ = {0.0f, 0.0f, -1.0f};
        vertices[2].normal_ = {0.0f, 0.0f, -1.0f};
        vertices[3].normal_ = {0.0f, 0.0f, -1.0f};

        vertices[0].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[1].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[2].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[3].tangent_ = {-1.0f, 0.0f, 0.0f};

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(3);
        indices.push_back(1);

        // Top
        vertices[4].position_ = {1.0f, 1.0f, 1.0f}; // Front Left
        vertices[5].position_ = {-1.0f, 1.0f, -1.0f}; // Back Right
        vertices[6].position_ = {1.0f, 1.0f, -1.0f}; // Back Left
        vertices[7].position_ = {-1.0f, 1.0f, 1.0f}; // Front Right

        vertices[4].tex_coord_ = {-1.0f, 1.0f, -1.0f};
        vertices[5].tex_coord_ = {1.0f, 1.0f, 1.0f};
        vertices[6].tex_coord_ = {-1.0f, 1.0f, 1.0f};
        vertices[7].tex_coord_ = {1.0f, 1.0f, -1.0f};

        vertices[4].normal_ = {0.0f, -1.0f, 0.0f};
        vertices[5].normal_ = {0.0f, -1.0f, 0.0f};
        vertices[6].normal_ = {0.0f, -1.0f, 0.0f};
        vertices[7].normal_ = {0.0f, -1.0f, 0.0f};

        vertices[4].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[5].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[6].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[7].tangent_ = {-1.0f, 0.0f, 0.0f};

        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(4);
        indices.push_back(7);
        indices.push_back(5);

        // Down
        vertices[8].position_ = {1.0f, -1.0f, -1.0f}; // Back Left
        vertices[9].position_ = {-1.0f, -1.0f, 1.0f}; // Front Right
        vertices[10].position_ = {1.0f, -1.0f, 1.0f}; // Front Left
        vertices[11].position_ = {-1.0f, -1.0f, -1.0f}; // Back Right

        vertices[8].tex_coord_ = {-1.0f, -1.0f, 1.0f};
        vertices[9].tex_coord_ = {1.0f, -1.0f, -1.0f};
        vertices[10].tex_coord_ = {-1.0f, -1.0f, -1.0f};
        vertices[11].tex_coord_ = {1.0f, -1.0f, 1.0f};

        vertices[8].normal_ = {0.0f, 1.0f, 0.0f};
        vertices[9].normal_ = {0.0f, 1.0f, 0.0f};
        vertices[10].normal_ = {0.0f, 1.0f, 0.0f};
        vertices[11].normal_ = {0.0f, 1.0f, 0.0f};

        vertices[8].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[9].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[10].tangent_ = {-1.0f, 0.0f, 0.0f};
        vertices[11].tangent_ = {-1.0f, 0.0f, 0.0f};

        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(10);
        indices.push_back(8);
        indices.push_back(11);
        indices.push_back(9);

        // Left
        vertices[12].position_ = {1.0f, -1.0f, -1.0f}; // Back Bottom
        vertices[13].position_ = {1.0f, 1.0f, 1.0f}; // Front Top
        vertices[14].position_ = {1.0f, 1.0f, -1.0f}; // Back Top
        vertices[15].position_ = {1.0f, -1.0f, 1.0f}; // Front Bottom

        // Cubemaps use left-hand coordinate system instead of right-hand coordinate system.
        vertices[12].tex_coord_ = {-1.0f, 1.0f, -1.0f};
        vertices[13].tex_coord_ = {-1.0f, -1.0f, 1.0f};
        vertices[14].tex_coord_ = {-1.0f, -1.0f, -1.0f};
        vertices[15].tex_coord_ = {-1.0f, 1.0f, 1.0f};

        vertices[12].normal_ = {-1.0f, 0.0f, 0.0f};
        vertices[13].normal_ = {-1.0f, 0.0f, 0.0f};
        vertices[14].normal_ = {-1.0f, 0.0f, 0.0f};
        vertices[15].normal_ = {-1.0f, 0.0f, 0.0f};

        vertices[12].tangent_ = {0.0f, 0.0f, 1.0f};
        vertices[13].tangent_ = {0.0f, 0.0f, 1.0f};
        vertices[14].tangent_ = {0.0f, 0.0f, 1.0f};
        vertices[15].tangent_ = {0.0f, 0.0f, 1.0f};

        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(14);
        indices.push_back(12);
        indices.push_back(15);
        indices.push_back(13);

        // Right
        vertices[16].position_ = {-1.0f, -1.0f, 1.0f}; // Front Bottom
        vertices[17].position_ = {-1.0f, 1.0f, -1.0f}; // Back Top
        vertices[18].position_ = {-1.0f, 1.0f, 1.0f}; // Front Top
        vertices[19].position_ = {-1.0f, -1.0f, -1.0f}; // Back Bottom

        // Cubemaps use left-hand coordinate system instead of right-hand coordinate system.
        vertices[16].tex_coord_ = {1.0f, 1.0f, 1.0f};
        vertices[17].tex_coord_ = {1.0f, -1.0f, -1.0f};
        vertices[18].tex_coord_ = {1.0f, -1.0f, 1.0f};
        vertices[19].tex_coord_ = {1.0f, 1.0f, -1.0f};

        vertices[16].normal_ = {1.0f, 0.0f, 0.0f};
        vertices[17].normal_ = {1.0f, 0.0f, 0.0f};
        vertices[18].normal_ = {1.0f, 0.0f, 0.0f};
        vertices[19].normal_ = {1.0f, 0.0f, 0.0f};

        vertices[16].tangent_ = {0.0f, 0.0f, -1.0f};
        vertices[17].tangent_ = {0.0f, 0.0f, -1.0f};
        vertices[18].tangent_ = {0.0f, 0.0f, -1.0f};
        vertices[19].tangent_ = {0.0f, 0.0f, -1.0f};

        indices.push_back(16);
        indices.push_back(17);
        indices.push_back(18);
        indices.push_back(16);
        indices.push_back(19);
        indices.push_back(17);

        // Back
        vertices[20].position_ = {-1.0f, -1.0f, -1.0f}; // Bottom Right
        vertices[21].position_ = {1.0f, 1.0f, -1.0f}; // Top Left
        vertices[22].position_ = {-1.0f, 1.0f, -1.0f}; // Top Right
        vertices[23].position_ = {1.0f, -1.0f, -1.0f}; // Bottom Left

        vertices[20].tex_coord_ = {1.0f, 1.0f, -1.0f};
        vertices[21].tex_coord_ = {-1.0f, -1.0f, -1.0f};
        vertices[22].tex_coord_ = {1.0f, -1.0f, -1.0f};
        vertices[23].tex_coord_ = {-1.0f, 1.0f, -1.0f};

        vertices[20].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[21].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[22].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[23].normal_ = {0.0f, 0.0f, 1.0f};

        vertices[20].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[21].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[22].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[23].tangent_ = {1.0f, 0.0f, 0.0f};

        indices.push_back(20);
        indices.push_back(21);
        indices.push_back(22);
        indices.push_back(20);
        indices.push_back(23);
        indices.push_back(21);

        return std::make_unique<mesh>("skybox", vertices, indices);
    }

    std::unique_ptr<mesh> asset_loader::make_screen_quad() {
        std::vector<vertex> vertices(4);

        vertices[0].position_ = {-1.0f, -1.0f, 0.0f};
        vertices[1].position_ = {1.0f, 1.0f, 0.0f};
        vertices[2].position_ = {-1.0f, 1.0f, 0.0f};
        vertices[3].position_ = {1.0f, -1.0f, 0.0f};

        vertices[0].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[1].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[2].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[3].normal_ = {0.0f, 0.0f, 1.0f};

        vertices[0].tex_coord_ = {0.0f, 0.0f, 0.0f};
        vertices[1].tex_coord_ = {1.0f, 1.0f, 0.0f};
        vertices[2].tex_coord_ = {0.0f, 1.0f, 0.0f};
        vertices[3].tex_coord_ = {1.0f, 0.0f, 0.0f};

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

        return std::make_unique<mesh>("screen_quad", vertices, indices);
    }

    std::unique_ptr<mesh> asset_loader::make_triangle(const std::string& _name) {
        std::vector<vertex> vertices(3);

        vertices[0].position_ = {-0.5f, -0.5f, 0.0f};
        vertices[1].position_ = {0.5f, 0.5f, 0.0f};
        vertices[2].position_ = {-0.5f, 0.5f, 0.0f};

        vertices[0].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[1].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[2].normal_ = {0.0f, 0.0f, 1.0f};

        vertices[0].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[1].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[2].tangent_ = {1.0f, 0.0f, 0.0f};

        vertices[0].tex_coord_ = {0.0f, 0.0f, 0.0f};
        vertices[1].tex_coord_ = {1.0f, 1.0f, 0.0f};
        vertices[2].tex_coord_ = {0.0f, 1.0f, 0.0f};

        std::vector<uint32_t> indices(3);

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        return std::make_unique<mesh>(_name, vertices, indices);
    }

    std::unique_ptr<mesh> asset_loader::make_quad(const std::string& _name) {
        std::vector<vertex> vertices(4);

        vertices[0].position_ = {-0.5f, -0.5f, 0.0f};
        vertices[1].position_ = {0.5f, 0.5f, 0.0f};
        vertices[2].position_ = {-0.5f, 0.5f, 0.0f};
        vertices[3].position_ = {0.5f, -0.5f, 0.0f};

        vertices[0].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[1].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[2].normal_ = {0.0f, 0.0f, 1.0f};
        vertices[3].normal_ = {0.0f, 0.0f, 1.0f};

        vertices[0].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[1].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[2].tangent_ = {1.0f, 0.0f, 0.0f};
        vertices[3].tangent_ = {1.0f, 0.0f, 0.0f};

        vertices[0].tex_coord_ = {0.0f, 0.0f, 0.0f};
        vertices[1].tex_coord_ = {1.0f, 1.0f, 0.0f};
        vertices[2].tex_coord_ = {0.0f, 1.0f, 0.0f};
        vertices[3].tex_coord_ = {1.0f, 0.0f, 0.0f};

        std::vector<uint32_t> indices(6);

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        indices[3] = 0;
        indices[4] = 3;
        indices[5] = 1;

        return std::make_unique<mesh>(_name, vertices, indices);
    }

    mesh* asset_loader::load_obj(const std::string& _name, const std::string& _file) {
        // Vertex Data and Index Data
        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;

        // Vertex Attribute(s)
        std::vector<vector3> positions;
        std::vector<vector3> normals;
        std::vector<vector3> tex_coords;

        // Read the file.
        std::string file_str = file_util::file_to_str(_file);
        std::stringstream ss(file_str.c_str());
        while (ss.good()) {
            std::string line;
            std::getline(ss, line, '\n');

            // Position
            if (std::strncmp("v ", line.c_str(), 2) == 0) {
                vector3 position;
                std::sscanf(line.c_str() + 2, "%f%f%f", &position.x_, &position.y_, &position.z_);
                positions.push_back(position);
            }

            // TexCoord
            if (std::strncmp("vt ", line.c_str(), 3) == 0) {
                vector3 tex_coord;
                std::sscanf(line.c_str() + 3, "%f%f", &tex_coord.x_, &tex_coord.y_);
                tex_coords.push_back(tex_coord);
            }

            // Normal
            if (std::strncmp("vn ", line.c_str(), 3) == 0) {
                vector3 normal;
                std::sscanf(line.c_str() + 3, "%f%f%f", &normal.x_, &normal.y_, &normal.z_);
                normals.push_back(normal);
            }

            // Faces
            if (std::strncmp("f ", line.c_str(), 2) == 0) {
                uint32_t position_indices[3];
                uint32_t tex_coord_indices[3];
                uint32_t normal_indices[3];

                int32_t matches = std::sscanf(
                        line.c_str() + 2,
                        "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                        &position_indices[0], &tex_coord_indices[0], &normal_indices[0],
                        &position_indices[1], &tex_coord_indices[1], &normal_indices[1],
                        &position_indices[2], &tex_coord_indices[2], &normal_indices[2]
                );

                if (matches != 9) {
                    std::string msg = "Cannot load mesh [" + _file + "]. Make sure that the mesh is triangulated!";
                    log::error(msg);
                    return nullptr;
                }

                // Add the Vertex.
                for (uint32_t i = 0; i < 3; ++i) {
                    vertex vertex;

                    vertex.position_ = positions[position_indices[i] - 1]; // -1 because .OBJ starts their index at 1.
                    vertex.normal_ = normals[normal_indices[i] - 1];
                    vertex.tex_coord_ = tex_coords[tex_coord_indices[i] - 1];

                    vertices.push_back(vertex);
                }
            }
        }

        // Index the vertices. (This defeats the point of indexing, since vertex data is still repeated, but creating a proper solution is too troublesome for now.)
        for (uint32_t i = 0; i < vertices.size(); ++i) {
            indices.push_back(i);
        }

        // Set Tangents
        for (uint32_t i = 0; i < indices.size(); i += 3) {
            // This gives us 2 of the 3 edges.
            matrix3x2 edges;
            edges[0][0] = vertices[indices[i + 1]].position_.x_ - vertices[indices[i]].position_.x_;
            edges[1][0] = vertices[indices[i + 1]].position_.y_ - vertices[indices[i]].position_.y_;
            edges[2][0] = vertices[indices[i + 1]].position_.z_ - vertices[indices[i]].position_.z_;
            edges[0][1] = vertices[indices[i + 2]].position_.x_ - vertices[indices[i]].position_.x_;
            edges[1][1] = vertices[indices[i + 2]].position_.y_ - vertices[indices[i]].position_.y_;
            edges[2][1] = vertices[indices[i + 2]].position_.z_ - vertices[indices[i]].position_.z_;

            // We have the length of 2 edges, and we know that each the edges can be represented by
            // some length of the tangent + some length of the bitangent.
            // One way we can find that length is by using the UV coordinates.
            // So for example, we know that for one of the edges,
            // the ratio of the edge along the tangent axis (also the same as the U axis for the texture coordinates) is U1 - U0 and.
            // the ratio of the edge along the bitangent axis (also the same as the U axis for the texture coordinates) is V1 - V0.
            matrix2x2 tangent_to_bitangent_ratio;
            tangent_to_bitangent_ratio[0][0] = vertices[indices[i + 1]].tex_coord_.x_ - vertices[indices[i]].tex_coord_.x_;
            tangent_to_bitangent_ratio[1][0] = vertices[indices[i + 1]].tex_coord_.y_ - vertices[indices[i]].tex_coord_.y_;
            tangent_to_bitangent_ratio[0][1] = vertices[indices[i + 2]].tex_coord_.x_ - vertices[indices[i]].tex_coord_.x_;
            tangent_to_bitangent_ratio[1][1] = vertices[indices[i + 2]].tex_coord_.y_ - vertices[indices[i]].tex_coord_.y_;

            matrix3x2 tangents = matrix_util::inverse_matrix(tangent_to_bitangent_ratio).value_or(matrix2x2::identity()) * edges;

            vertices[indices[i + 0]].tangent_ += vector3(tangents[0][0], tangents[1][0], tangents[2][0]);
            vertices[indices[i + 1]].tangent_ += vector3(tangents[0][0], tangents[1][0], tangents[2][0]);
            vertices[indices[i + 2]].tangent_ += vector3(tangents[0][0], tangents[1][0], tangents[2][0]);
        }

        for (auto& vert: vertices) {
            vert.tangent_.normalise();
        }

        // Create the mesh.
        meshes_[_name] = std::make_unique<mesh>(_name, vertices, indices);
        return meshes_[_name].get();
    }

    mesh* asset_loader::get_mesh(const std::string& _name) {
        auto iter = meshes_.find(_name);
        return (iter == meshes_.end()) ? nullptr : iter->second.get();
    }
}