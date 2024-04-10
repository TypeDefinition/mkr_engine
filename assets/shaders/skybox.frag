#version 460 core

// Outputs
layout (location = 0) out vec4 out_colour;

// Inputs
in io_block {
    vec3 io_tex_coord;
};

// Uniforms
uniform vec4 u_colour;
uniform bool u_texture_skybox_enabled;
uniform samplerCube u_texture_skybox;

void main() {
    gl_FragDepth = 1.0f; // Set the skybox to infinitely far away.
    out_colour = u_texture_skybox_enabled ? texture(u_texture_skybox, io_tex_coord) * u_colour : u_colour;
}