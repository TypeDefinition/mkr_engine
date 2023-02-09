#version 460 core

layout (location = 0) out vec4 out_composite;

// Inputs
in io_block {
    vec3 io_tex_coord;
};

uniform sampler2D u_texture_frag_position;
uniform sampler2D u_texture_frag_normal;
uniform sampler2D u_texture_depth_stencil;
uniform sampler2D u_texture_composite;

uniform float u_near;
uniform float u_far;
uniform vec2 u_bottom_left;
uniform vec2 u_top_right;

// Custom Uniform(s)
uniform int u_blur_intensity = 3;

void main() {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int x = -u_blur_intensity; x <= u_blur_intensity; ++x) {
        for (int y = -u_blur_intensity; y <= u_blur_intensity; ++y) {
            colour += texelFetch(u_texture_composite, ivec2(gl_FragCoord) + ivec2(x, y), 0);
        }
    }

    int numPixels = (u_blur_intensity + u_blur_intensity + 1) * (u_blur_intensity + u_blur_intensity + 1);
    colour.rgb /= float(numPixels);
    colour.a = 1.0f;

    out_composite = colour;
}