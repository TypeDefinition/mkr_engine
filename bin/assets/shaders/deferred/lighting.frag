#version 460 core

layout (location = 0) out vec4 out_colour;

// Inputs
in VS_OUT {
    vec2 tex_coord;
} vs_out;

#include <light.frag>
#include <shadow.frag>

// Transform
uniform mat4 u_inv_view_matrix;

// Textures
uniform sampler2D u_texture_position;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_specular;

void main() {
    const vec3 pos = texture(u_texture_position, vs_out.tex_coord).rgb;
    const vec3 norm = texture(u_texture_normal, vs_out.tex_coord).rgb;

    const vec4 diff_tex_val = texture(u_texture_diffuse, vs_out.tex_coord);
    const vec3 diff = diff_tex_val.rgb;
    const float alpha = diff_tex_val.a;

    const vec4 spec_tex_val = texture(u_texture_specular, vs_out.tex_coord);
    const vec3 spec = spec_tex_val.rgb;
    const float gloss = spec_tex_val.a;

    bool cast_shadow[max_lights];
    get_cast_shadow(pos, norm,
                    u_inv_view_matrix,
                    u_lights, u_num_lights,
                    cast_shadow);

    const vec3 ambient = diff * u_ambient_light.rgb;
    const vec3 diffuse = diff * get_light_diffuse(pos, norm, cast_shadow);
    const vec3 specular = spec * get_light_specular(pos, norm, gloss, cast_shadow);
    const vec3 colour = ambient + diffuse + specular;

    out_colour = vec4(colour, alpha);
}