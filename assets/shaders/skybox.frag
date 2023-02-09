#version 460 core

// Outputs
layout (location = 0) out vec4 out_composite;

// Inputs
in io_block {
    vec3 io_tex_coord;
};

// Uniforms
uniform vec4 u_sky_colour;
uniform bool u_texture_skybox_enabled;
uniform samplerCube u_texture_skybox;

void main() {
    out_composite = u_texture_skybox_enabled ? texture(u_texture_skybox, io_tex_coord) * u_sky_colour : u_sky_colour;
}