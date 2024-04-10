#version 460 core

layout (location = 0) out vec4 out_composite;

// Inputs
in io_block {
    vec2 io_tex_coord;
};

uniform sampler2D u_texture_frag_position;
uniform sampler2D u_texture_frag_normal;
uniform sampler2D u_texture_depth_stencil;
uniform sampler2D u_texture_composite;

uniform float u_near;
uniform float u_far;
uniform vec2 u_bottom_left;
uniform vec2 u_top_right;

void main() {
    vec4 colour = texture(u_texture_composite, io_tex_coord);
    colour.r = 1.0f - colour.r; colour.g = 1.0f - colour.g; colour.b = 1.0f - colour.b;
    out_composite = colour;
}