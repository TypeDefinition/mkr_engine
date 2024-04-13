#version 460 core

// Outputs
layout (location = 0) out vec4 out_colour;

// Inputs
in VS_OUT {
    vec3 tex_coord;
} vs_out;

// Uniforms
uniform vec4 u_colour;
uniform bool u_texture_skybox_enabled;
uniform samplerCube u_texture_skybox;

void main() {
    out_colour = u_texture_skybox_enabled ? texture(u_texture_skybox, vs_out.tex_coord) * u_colour : u_colour;
}