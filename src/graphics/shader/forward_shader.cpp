#include "graphics/shader/forward_shader.h"
#include "graphics/shader/texture_unit.h"

namespace mkr {
    void forward_shader::assign_uniforms() {
        // Transform
        uniform_handles_[uniform::u_view_matrix] = get_uniform_location("u_view_matrix");
        uniform_handles_[uniform::u_projection_matrix] = get_uniform_location("u_projection_matrix");
        uniform_handles_[uniform::u_texture_offset] = get_uniform_location("u_texture_offset");
        uniform_handles_[uniform::u_texture_scale] = get_uniform_location("u_texture_scale");

        uniform_handles_[uniform::u_inv_view_matrix] = get_uniform_location("u_inv_view_matrix");

        // Material
        uniform_handles_[uniform::u_diffuse_colour] = get_uniform_location("u_diffuse_colour");
        uniform_handles_[uniform::u_specular_colour] = get_uniform_location("u_specular_colour");
        uniform_handles_[uniform::u_displacement_scale] = get_uniform_location("u_displacement_scale");

        // Textures
        uniform_handles_[uniform::u_has_texture_diffuse] = get_uniform_location("u_has_texture_diffuse");
        uniform_handles_[uniform::u_has_texture_specular] = get_uniform_location("u_has_texture_specular");
        uniform_handles_[uniform::u_has_texture_normal] = get_uniform_location("u_has_texture_normal");
        uniform_handles_[uniform::u_has_texture_displacement] = get_uniform_location("u_has_texture_displacement");

        uniform_handles_[uniform::u_texture_diffuse] = get_uniform_location("u_texture_diffuse");
        uniform_handles_[uniform::u_texture_specular] = get_uniform_location("u_texture_specular");
        uniform_handles_[uniform::u_texture_normal] = get_uniform_location("u_texture_normal");
        uniform_handles_[uniform::u_texture_displacement] = get_uniform_location("u_texture_displacement");

        // Shadows
        for (auto i = 0; i < lighting::max_lights; ++i) {
            uniform_handles_[i + uniform::u_texture_shadows0] = get_uniform_location("u_texture_shadows[" + std::to_string(i) + "]");
            uniform_handles_[i + uniform::u_cubemap_shadows0] = get_uniform_location("u_cubemap_shadows[" + std::to_string(i) + "]");
        }

        // Lights
        uniform_handles_[uniform::u_ambient_light] = get_uniform_location("u_ambient_light");
        uniform_handles_[uniform::u_num_lights] = get_uniform_location("u_num_lights");
        for (auto i = 0; i < lighting::max_lights; ++i) {
            uniform_handles_[i + uniform::u_light_mode0] = get_uniform_location("u_lights[" + std::to_string(i) + "].mode_");
            uniform_handles_[i + uniform::u_light_power0] = get_uniform_location("u_lights[" + std::to_string(i) + "].power_");
            uniform_handles_[i + uniform::u_light_colour0] = get_uniform_location("u_lights[" + std::to_string(i) + "].colour_");
            uniform_handles_[i + uniform::u_light_attenuation_constant0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_constant_");
            uniform_handles_[i + uniform::u_light_attenuation_linear0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_linear_");
            uniform_handles_[i + uniform::u_light_attenuation_quadratic0] = get_uniform_location("u_lights[" + std::to_string(i) + "].attenuation_quadratic_");
            uniform_handles_[i + uniform::u_light_spotlight_inner_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_inner_cosine_");
            uniform_handles_[i + uniform::u_light_spotlight_outer_cosine0] = get_uniform_location("u_lights[" + std::to_string(i) + "].spotlight_outer_cosine_");
            uniform_handles_[i + uniform::u_light_position0] = get_uniform_location("u_lights[" + std::to_string(i) + "].position_");
            uniform_handles_[i + uniform::u_light_direction0] = get_uniform_location("u_lights[" + std::to_string(i) + "].direction_");
            uniform_handles_[i + uniform::u_light_shadow_distance0] = get_uniform_location("u_lights[" + std::to_string(i) + "].shadow_distance_");
            uniform_handles_[i + uniform::u_light_view_projection_matrix0] = get_uniform_location("u_lights[" + std::to_string(i) + "].view_projection_matrix_");
        }
    }

    void forward_shader::assign_textures() {
        set_uniform(uniform::u_texture_diffuse, (int32_t) texture_unit::texture_diffuse);
        set_uniform(uniform::u_texture_specular, (int32_t) texture_unit::texture_specular);
        set_uniform(uniform::u_texture_normal, (int32_t) texture_unit::texture_normal);
        set_uniform(uniform::u_texture_displacement, (int32_t) texture_unit::texture_displacement);

        for (auto i = 0; i < lighting::max_lights; ++i) {
            set_uniform(i + uniform::u_texture_shadows0, (int32_t) (i + texture_unit::texture_shadows0));
            set_uniform(i + uniform::u_cubemap_shadows0, (int32_t) (i + texture_unit::cubemap_shadows0));
        }
    }
}