#include "graphics/shader/skybox_shader.h"

namespace mkr {
    skybox_shader::skybox_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
        : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
        assign_uniforms();
        assign_textures();
    }

    void skybox_shader::assign_uniforms() {
        // Vertex Shader
        uniform_handles_[uniform::u_view_projection_matrix] = get_uniform_location("u_view_projection_matrix");

        // Fragment Shader
        uniform_handles_[uniform::u_skybox_colour] = get_uniform_location("u_skybox_colour");
        uniform_handles_[uniform::u_texture_skybox_enabled] = get_uniform_location("u_texture_skybox_enabled");
        uniform_handles_[uniform::u_texture_skybox] = get_uniform_location("u_texture_skybox");
    }

    void skybox_shader::assign_textures() {
        set_uniform(uniform::u_texture_skybox, (int32_t)texture_unit::texture_skybox);
    }
} // mkr