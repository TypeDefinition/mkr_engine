#version 460 core

// Inputs
in io_block {
    vec3 io_tex_coord;
};

// Outputs
layout (location = 0) out vec4 io_frag_colour;

// Uniforms
uniform vec4 u_sky_colour;
uniform bool u_texture_skybox_enabled;
uniform samplerCube u_texture_skybox;

void main() {
    io_frag_colour = u_texture_skybox_enabled ? texture(u_texture_skybox, io_tex_coord) : u_sky_colour;
}