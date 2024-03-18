#pragma once

#include <cstring>
#include <memory>
#include <SDL2/SDL_image.h>
#include <log/log.h>
#include <maths/matrix_util.h>
#include "graphics/mesh/mesh.h"
#include "util/file_util.h"

namespace mkr {
    class mesh_builder {
    public:
        mesh_builder() = delete;

        static std::unique_ptr<mesh> make_skybox(const std::string& _name) {
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

            return std::make_unique<mesh>(_name, vertices, indices);
        }

        static std::unique_ptr<mesh> make_screen_quad(const std::string& _name) {
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

            return std::make_unique<mesh>(_name, vertices, indices);
        }

        static std::unique_ptr<mesh> load_obj(const std::string &_name, const std::string &_file) {
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
            auto m = std::make_unique<mesh>(_name, vertices, indices);
            return m;
        }
    };
} // mkr