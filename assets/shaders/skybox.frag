#version 460 core

// Inputs
in io_block {
    vec3 io_tex_coord;
};

// Outputs
layout (location = 0) out vec4 io_frag_colour;

// Uniforms
uniform samplerCube u_texture_skybox;

void main() {
    io_frag_colour =  texture(u_texture_skybox, io_tex_coord);
}