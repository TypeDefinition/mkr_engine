#version 460 core

// Per-Vertex Inputs
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_tex_coord;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_tangent;

// Per-Instance Inputs
layout(location = 4) in mat4 v_model_matrix; // The max size of a vertex attribute is vec4. A mat4 is the size of 4 vec4s. Since this is a mat4, it takes up attribute 4, 5, 6 and 7.

// Outputs
out vec3 io_tex_coord;

// Uniforms
uniform mat4 u_view_projection_matrix;

void main() {
    io_tex_coord = v_tex_coord;
    gl_Position = u_view_projection_matrix * v_model_matrix * vec4(v_position, 1.0);
}