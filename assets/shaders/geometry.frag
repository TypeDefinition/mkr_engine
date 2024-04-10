#version 460 core

// IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin.
layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out vec4 out_specular;
layout (location = 4) out float out_gloss;

// Inputs
in io_block {
    vec2 io_tex_coord;
    vec3 io_position;// Vertex position in camera space.
    vec3 io_normal;// Vertex normal in camera space.
    mat3 io_tbn_matrix;// Converts from tangent space to camera space.
};

// Material
uniform vec4 u_diffuse_colour;
uniform vec4 u_specular_colour;
uniform float u_gloss;
uniform float u_displacement_scale;

// Textures
uniform bool u_texture_diffuse_enabled;
uniform bool u_texture_normal_enabled;
uniform bool u_texture_specular_enabled;
uniform bool u_texture_gloss_enabled;
uniform bool u_texture_displacement_enabled;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_gloss;
uniform sampler2D u_texture_displacement;

// Includes
vec2 parallax_occlusion(const in sampler2D _texture, const in vec2 _tex_coord, float _disp_scale, const in vec3 _frag_pos, const in mat3 _inv_tbn_matrix, const in vec3 _normal);

vec3 get_normal(const in vec2 _tex_coord) {
    if (u_texture_normal_enabled) {
        const vec3 normal = 2.0f * texture(u_texture_normal, _tex_coord).rgb - vec3(1.0f, 1.0f, 1.0f); // Convert into the [-1, 1] range.
        return io_tbn_matrix * normal; // Convert the normal from tangent space to camera space.
    }
    return io_normal;
}

float get_gloss(const in vec2 _tex_coord) {
    return u_texture_gloss_enabled ? texture(u_texture_gloss, _tex_coord).r * u_gloss : u_gloss;
}

vec4 get_diffuse(const in vec2 _tex_coord) {
    return u_texture_diffuse_enabled ? texture(u_texture_diffuse, _tex_coord) * u_diffuse_colour : u_diffuse_colour;
}

vec4 get_specular(const in vec2 _tex_coord) {
    return u_texture_specular_enabled ? texture(u_texture_specular, _tex_coord) * u_specular_colour : u_specular_colour;
}

vec2 get_tex_coord() {
    return u_texture_displacement_enabled ? parallax_occlusion(u_texture_displacement, io_tex_coord, u_displacement_scale, io_position, io_tbn_matrix, io_normal) : io_tex_coord;
}

void main() {
    const vec2 tex_coord = get_tex_coord();

    out_position = io_position;
    out_normal = get_normal(tex_coord);
    out_diffuse = get_diffuse(tex_coord);
    out_specular = get_specular(tex_coord);
    out_gloss = get_gloss(tex_coord);
}