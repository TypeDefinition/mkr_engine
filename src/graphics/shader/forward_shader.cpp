#include "graphics/shader/forward_shader.h"

namespace mkr {
    forward_shader::forward_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
        : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
        assign_uniforms();
        assign_textures();
    }

    void forward_shader::assign_uniforms() {
        // Vertex Shader
        uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
        uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");
        uniform_handles_[uniform::u_view_projection_matrix] = get_uniform_location("u_view_projection_matrix");
        uniform_handles_[uniform::u_texture_offset] = get_uniform_location("u_texture_offset");
        uniform_handles_[uniform::u_texture_scale] = get_uniform_location("u_texture_scale");

        // Fragment Shader
        uniform_handles_[uniform::u_diffuse_colour] = get_uniform_location("u_diffuse_colour");
        uniform_handles_[uniform::u_specular_colour] = get_uniform_location("u_specular_colour");
        uniform_handles_[uniform::u_gloss] = get_uniform_location("u_gloss");
        uniform_handles_[uniform::u_displacement_scale] = get_uniform_location("u_displacement_scale");

        uniform_handles_[uniform::u_texture_diffuse_enabled] = get_uniform_location("u_texture_diffuse_enabled");
        uniform_handles_[uniform::u_texture_normal_enabled] = get_uniform_location("u_texture_normal_enabled");
        uniform_handles_[uniform::u_texture_specular_enabled] = get_uniform_location("u_texture_specular_enabled");
        uniform_handles_[uniform::u_texture_gloss_enabled] = get_uniform_location("u_texture_gloss_enabled");
        uniform_handles_[uniform::u_texture_displacement_enabled] = get_uniform_location("u_texture_displacement_enabled");

        uniform_handles_[uniform::u_texture_diffuse] = get_uniform_location("u_texture_diffuse");
        uniform_handles_[uniform::u_texture_normal] = get_uniform_location("u_texture_normal");
        uniform_handles_[uniform::u_texture_specular] = get_uniform_location("u_texture_specular");
        uniform_handles_[uniform::u_texture_gloss] = get_uniform_location("u_texture_gloss");
        uniform_handles_[uniform::u_texture_displacement] = get_uniform_location("u_texture_displacement");

        uniform_handles_[uniform::u_ambient_light] = get_uniform_location("u_ambient_light");
        uniform_handles_[uniform::u_num_lights] = get_uniform_location("u_num_lights");
        for (auto i = 0; i < max_lights; ++i) {
            uniform_handles_[i + uniform::u_light_mode0] = get_uniform_location("u_lights[" + std::to_string(i) + "].mode_");
            uniform_handles_[i + uniform::u_light_power0] = get_uniform_location("u_lights[" + std::to_string(i) + "].power_");
            uniform_handles_[i + uniform::u_light_colour0] = get_uniform_location("u_lights[" + std::to_string(i) + "].colour_");
            uniform_handles_[i + uniform::u_light_attenuation_constant0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_constant_");
            uniform_handles_[i + uniform::u_light_attenuation_linear0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_linear_");
            uniform_handles_[i + uniform::u_light_attenuation_quadratic0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_quadratic_");
            uniform_handles_[i + uniform::u_light_spotlight_inner_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_inner_cosine_");
            uniform_handles_[i + uniform::u_light_spotlight_outer_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_outer_cosine_");
            uniform_handles_[i + uniform::u_light_position_camera_space0] = get_uniform_location("u_lights[" + std::to_string(i) + "].position_camera_space_");
            uniform_handles_[i + uniform::u_light_direction_camera_space0] = get_uniform_location("u_lights[" + std::to_string(i) + "].direction_camera_space_");
        }
    }

    void forward_shader::assign_textures() {
        set_uniform(uniform::u_texture_diffuse, (int32_t)texture_unit::texture_diffuse);
        set_uniform(uniform::u_texture_normal, (int32_t)texture_unit::texture_normal);
        set_uniform(uniform::u_texture_specular, (int32_t)texture_unit::texture_specular);
        set_uniform(uniform::u_texture_gloss, (int32_t)texture_unit::texture_gloss);
        set_uniform(uniform::u_texture_displacement, (int32_t)texture_unit::texture_displacement);
    }
}