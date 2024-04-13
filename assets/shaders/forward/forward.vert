#version 460 core

/* IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin. */

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
    vec2 tex_coord;
    vec3 position; // Vertex position in camera space.
    vec3 normal; // Vertex normal in camera space.
    mat3 tbn_matrix; // Converts from tangent space to camera space.
} vs_out;

// Uniforms
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform vec2 u_texture_offset;
uniform vec2 u_texture_scale;

void main() {
    vec3 n = normalize(v_normal_matrix * v_normal);
    vec3 t = normalize(v_normal_matrix * v_tangent);
    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(n, t);

    vs_out.tex_coord = (v_tex_coord + u_texture_offset) * u_texture_scale;
    vs_out.position = (u_view_matrix * v_model_matrix * vec4(v_position, 1.0f)).xyz;
    vs_out.normal = n;
    vs_out.tbn_matrix = mat3(t, b, n);

    gl_Position = u_projection_matrix * u_view_matrix * v_model_matrix * vec4(v_position, 1.0f);
}