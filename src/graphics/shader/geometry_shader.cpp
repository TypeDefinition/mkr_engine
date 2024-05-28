#include "graphics/shader/geometry_shader.h"
#include "graphics/shader/texture_unit.h"

namespace mkr {
    geometry_shader::geometry_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
        : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
        assign_uniforms();
        assign_textures();
    }

    void geometry_shader::assign_uniforms() {
        // Vertex Shader
        uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
        uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");
        uniform_handles_[uniform::u_texture_offset] = get_uniform_location("u_texture_offset");
        uniform_handles_[uniform::u_texture_scale] = get_uniform_location("u_texture_scale");

        // Fragment Shader
        uniform_handles_[uniform::u_diffuse_colour] = get_uniform_location("u_diffuse_colour");
        uniform_handles_[uniform::u_specular_colour] = get_uniform_location("u_specular_colour");
        uniform_handles_[uniform::u_displacement_scale] = get_uniform_location("u_displacement_scale");

        uniform_handles_[uniform::u_has_texture_diffuse] = get_uniform_location("u_has_texture_diffuse");
        uniform_handles_[uniform::u_has_texture_normal] = get_uniform_location("u_has_texture_normal");
        uniform_handles_[uniform::u_has_texture_specular] = get_uniform_location("u_has_texture_specular");
        uniform_handles_[uniform::u_has_texture_displacement] = get_uniform_location("u_has_texture_displacement");

        uniform_handles_[uniform::u_texture_diffuse] = get_uniform_location("u_texture_diffuse");
        uniform_handles_[uniform::u_texture_normal] = get_uniform_location("u_texture_normal");
        uniform_handles_[uniform::u_texture_specular] = get_uniform_location("u_texture_specular");
        uniform_handles_[uniform::u_texture_displacement] = get_uniform_location("u_texture_displacement");
    }

    void geometry_shader::assign_textures() {
        set_uniform(uniform::u_texture_diffuse, (int32_t) texture_unit::texture_diffuse);
        set_uniform(uniform::u_texture_normal, (int32_t) texture_unit::texture_normal);
        set_uniform(uniform::u_texture_specular, (int32_t) texture_unit::texture_specular);
        set_uniform(uniform::u_texture_displacement, (int32_t) texture_unit::texture_displacement);
    }
} // mkr