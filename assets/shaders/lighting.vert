#version 460 core

// Per-Vertex Inputs
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_tex_coord;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_tangent;

// Outputs
out VS_OUT {
    vec2 tex_coord;
} vs_out;

void main() {
    vs_out.tex_coord = v_tex_coord;
    gl_Position = vec4(v_position, 1.0f);
}