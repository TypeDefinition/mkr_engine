#version 460 core

// Inputs
in vec3 io_tex_coord;

// Outputs
layout (location = 0) out vec4 io_frag_colour;

// Uniforms
uniform samplerCube texture_skybox;

void main()
{
    io_frag_colour =  texture(texture_skybox, io_tex_coord);
}