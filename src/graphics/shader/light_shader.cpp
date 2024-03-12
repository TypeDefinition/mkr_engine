#include "graphics/shader/light_shader.h"

namespace mkr {
    light_shader::light_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
        : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
        assign_uniforms();
        assign_textures();
    }

    void light_shader::assign_uniforms() {
        // Fragment Shader
        uniform_handles_[uniform::u_texture_frag_position] = get_uniform_location("u_texture_frag_position");
        uniform_handles_[uniform::u_texture_frag_normal] = get_uniform_location("u_texture_frag_normal");
        uniform_handles_[uniform::u_texture_frag_diffuse] = get_uniform_location("u_texture_frag_diffuse");
        uniform_handles_[uniform::u_texture_frag_specular] = get_uniform_location("u_texture_frag_specular");
        uniform_handles_[uniform::u_texture_frag_gloss] = get_uniform_location("u_texture_frag_gloss");

        uniform_handles_[uniform::u_texture_light_diffuse] = get_uniform_location("u_texture_light_diffuse");
        uniform_handles_[uniform::u_texture_light_specular] = get_uniform_location("u_texture_light_specular");

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

    void light_shader::assign_textures() {
        set_uniform(uniform::u_texture_frag_position, (int32_t)texture_unit::texture_frag_position);
        set_uniform(uniform::u_texture_frag_normal, (int32_t)texture_unit::texture_frag_normal);
        set_uniform(uniform::u_texture_frag_diffuse, (int32_t)texture_unit::texture_frag_diffuse);
        set_uniform(uniform::u_texture_frag_specular, (int32_t)texture_unit::texture_frag_specular);
        set_uniform(uniform::u_texture_frag_gloss, (int32_t)texture_unit::texture_frag_gloss);
        set_uniform(uniform::u_texture_light_diffuse, (int32_t)texture_unit::texture_light_diffuse);
        set_uniform(uniform::u_texture_light_specular, (int32_t)texture_unit::texture_light_specular);
    }
} // mkr