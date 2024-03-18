#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class skybox_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            u_view_projection_matrix,

            u_skybox_colour,
            u_texture_skybox_enabled,
            u_texture_skybox,

            num_shader_uniforms,
        };

    protected:
        void assign_uniforms();

        void assign_textures();

    public:
        skybox_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources);

        virtual ~skybox_shader() {}
    };
} // mkr