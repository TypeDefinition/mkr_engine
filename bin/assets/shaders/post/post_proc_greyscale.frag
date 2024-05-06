#version 460 core

layout (location = 0) out vec4 out_composite;

// Inputs
in VS_OUT {
    vec2 tex_coord;
} vs_out;

uniform sampler2D u_texture_frag_position;
uniform sampler2D u_texture_frag_normal;
uniform sampler2D u_texture_depth_stencil;
uniform sampler2D u_texture_composite;

uniform float u_near;
uniform float u_far;
uniform vec2 u_bottom_left;
uniform vec2 u_top_right;

void main() {
    // This formula is just a common proven formula that can be found on the internet.
    // Basically it is based on the fact that humans see green more then any other colour.
    vec4 colour = texture(u_texture_composite, vs_out.tex_coord);
    float brightness = (0.21f * colour.r) + (0.72f * colour.g) + (0.07f * colour.b);
    out_composite = vec4(brightness, brightness, brightness, 1.0f);
}