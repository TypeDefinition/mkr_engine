#version 460 core

// Per-Vertex Inputs
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_tex_coord;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_tangent;

// Per-Instance Inputs
layout(location = 4) in mat4 v_model_matrix; // The max size of a vertex attribute is vec4. A mat4 is the size of 4 vec4s. Since this is a mat4, it takes up attribute 4, 5, 6 and 7.
layout(location = 8) in mat3 v_normal_matrix; // The max size of a vertex attribute is vec4. A mat3 is the size of 3 vec3s. Since this is a mat3, it takes up attribute 8, 9, and 10.

// Outputs
out VS_OUT {
    vec3 tex_coord;
} vs_out;

// Uniforms
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

void main() {
    vs_out.tex_coord = vec3(-v_position.x, v_position.yz); // Cubemap coordinates are left-handed.
    gl_Position = u_projection_matrix * u_view_matrix * vec4(v_position, 1.0);
    gl_Position.z = gl_Position.w; // Ensures that gl_FragDepth will always be 1.
}