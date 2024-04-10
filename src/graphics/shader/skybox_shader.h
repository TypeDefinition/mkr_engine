#pragma once

#include <cstdint>
#include "graphics/shader/shader_program.h"
#include "graphics/shader/texture_unit.h"

namespace mkr {
    class skybox_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            // Transform
            u_view_matrix,
            u_projection_matrix,

            // Material
            u_colour,

            // Texture
            u_texture_skybox_enabled,
            u_texture_skybox,

            num_shader_uniforms,
        };

    protected:
        void assign_uniforms() {
            // Vertex Shader
            uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
            uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");

            // Fragment Shader
            uniform_handles_[uniform::u_colour] = get_uniform_location("u_colour");
            uniform_handles_[uniform::u_texture_skybox_enabled] = get_uniform_location("u_texture_skybox_enabled");
            uniform_handles_[uniform::u_texture_skybox] = get_uniform_location("u_texture_skybox");
        }

        void assign_textures() {
            set_uniform(uniform::u_texture_skybox, (int32_t) texture_unit::texture_skybox);
        }

    public:
        skybox_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
            : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
            assign_uniforms();
            assign_textures();
        }

        virtual ~skybox_shader() {}
    };
} // mkr