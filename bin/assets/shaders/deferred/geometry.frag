#version 460 core

// IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin.
layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out vec4 out_specular;

// Inputs
in VS_OUT {
    vec2 tex_coord;
    vec3 position;// Vertex position in camera space.
    vec3 normal;// Vertex normal in camera space.
    mat3 tbn_matrix;// Converts from tangent space to camera space.
} vs_out;

#include <parallax.frag>

// Material
uniform vec4 u_diffuse_colour;
uniform vec4 u_specular_colour;
uniform float u_displacement_scale;

// Textures
uniform bool u_has_texture_diffuse;
uniform bool u_has_texture_normal;
uniform bool u_has_texture_specular;
uniform bool u_has_texture_displacement;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_displacement;

vec2 get_tex_coord() {
    return u_has_texture_displacement ? parallax(u_texture_displacement, vs_out.tex_coord, u_displacement_scale, vs_out.position, vs_out.tbn_matrix, vs_out.normal) : vs_out.tex_coord;
}

vec3 get_normal(const in vec2 _tex_coord) {
    if (u_has_texture_normal) {
        const vec3 normal = 2.0f * texture(u_texture_normal, _tex_coord).rgb - vec3(1.0f, 1.0f, 1.0f); // Convert into the [-1, 1] range.
        return vs_out.tbn_matrix * normal; // Convert the normal from tangent space to camera space.
    }
    return vs_out.normal;
}

vec4 get_diffuse(const in vec2 _tex_coord) {
    return u_has_texture_diffuse ? texture(u_texture_diffuse, _tex_coord) * u_diffuse_colour : u_diffuse_colour;
}

vec4 get_specular(const in vec2 _tex_coord) {
    return u_has_texture_specular ? texture(u_texture_specular, _tex_coord) * u_specular_colour : u_specular_colour;
}

void main() {
    const vec2 tex_coord = get_tex_coord();

    out_position = vs_out.position;
    out_normal = get_normal(tex_coord);
    out_diffuse = get_diffuse(tex_coord);
    out_specular = get_specular(tex_coord);
}