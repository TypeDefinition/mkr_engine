#version 460 core

/* IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin. */

// Per-Vertex Inputs
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_tex_coord;
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec3 v_tangent;

// Per-Instance Inputs
layout(location = 4) in mat4 v_model_matrix; // The max size of a vertex attribute is vec4. A mat4 is the size of 4 vec4s. Since this is a mat4, it takes up attribute 4, 5, 6 and 7.
layout(location = 8) in mat3 v_normal_matrix; // The max size of a vertex attribute is vec4. A mat3 is the size of 3 vec3s. Since this is a mat3, it takes up attribute 8, 9, and 10.

// Outputs
out io_block {
    vec3 io_tex_coord;
    vec3 io_vertex_position; // Vertex position in camera space.
    vec3 io_vertex_normal; // Vertex normal in camera space.
    mat3 io_normal_matrix;
    mat3 io_tbn_matrix; // Converts from tangent space to camera space.
    mat3 io_tbn_inv_matrix; // Converts from camera space to tangent space.
};

// Uniforms
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_projection_matrix;

void main() {
    io_tex_coord = v_tex_coord;

    mat4 model_view_matrix = u_view_matrix * v_model_matrix;
    io_vertex_position = (model_view_matrix * vec4(v_position, 1.0f)).xyz;
    io_vertex_normal = normalize(v_normal_matrix * v_normal);
    io_normal_matrix = v_normal_matrix;

    // This is the Gramm-Schmidt process. dot(Tangent, Normal) gives us the length of the projection of the tangent along the normal vector.
    // The product of this length by the normal itself is the component of the tangent along the normal.
    // Substract that from the tangent and we get a new vector which is perpendicular to the normal.
    // This is our new tangent (just remember to normalize it as well...).
    vec3 tangent = normalize(v_tangent - dot(v_tangent, v_normal) * v_normal);
    // A cross product between the tangent and the normal gives us the bitangent.
    vec3 bitangent = normalize(cross(v_normal, tangent));

    // io_tbn_matrix[0] = v_normal_matrix * normalize(tangent);
    // io_tbn_matrix[1] = v_normal_matrix * normalize(bitangent);
    io_tbn_matrix[0] = normalize(v_normal_matrix * tangent);
    io_tbn_matrix[1] = normalize(v_normal_matrix * bitangent);
    io_tbn_matrix[2] = io_vertex_normal;
    io_tbn_inv_matrix = transpose(io_tbn_matrix); // Since TBN is an orthogonal matrix, its transpose is also its inverse.

    gl_Position = u_view_projection_matrix * v_model_matrix * vec4(v_position, 1.0f);
}