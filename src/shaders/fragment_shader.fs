#version 460 core

// Inputs
in vec2 io_tex_coord;

// Outputs
out vec4 io_frag_colour;

// Uniforms
uniform sampler2D texture_albedo;

void main()
{
    io_frag_colour = texture(texture_albedo, io_tex_coord);
}