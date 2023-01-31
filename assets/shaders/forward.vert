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
    mat3 io_tbn_matrix;
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

    /* So, at this point you might be asking, "If we want to convert a vector from one space to another, we need to dot product it with all 3 axis of the new space.
    So if we take CBT_V_MATRIX_TBN and multiply it by a vec3 right now, we aren't actually doing a dot product with all 3 axis. We need to transpose CBT_V_MATRIX_TBN in order to achieve that."
    Ah, but you see, the transpose of CBT_V_MATRIX_TBN converts a point TO tangent space. But what we want is to actually convert FROM tangent space. And for orthoganal matrices, their transpose is also
    their inverse. Which is why CBT_V_MATRIX_TBN is currently the tranpose of what it is now. Because we want it to do the opposite of converting to tangent space. We want to convert FROM tangent space
    to view space. Later on in the fragment shader you will see a point where we transpose CBT_V_MATRIX_TBN when we want to convert TO tangent space. So at the end of the day I guess it kinda is cause
    we named CBT_V_MATRIX_TBN poorly. We should actually name it CBT_V_MATRIX_TBNInverse. But I've already typed out the code and this entire freaking passage. So we'll just keep the name. And if you
    are wondering why I typed this whole message instead of changing the name of CBT_V_MATRIX_TBN, it's cause I only thought of the name change as I am typing this out. */
    // TBN (Technically the inverse of a TBN Matrix. Converts from tangent space to view space.)
    io_tbn_matrix[0] = normalize(v_normal_matrix *  tangent);
    io_tbn_matrix[1] = normalize(v_normal_matrix *  bitangent);
    io_tbn_matrix[2] = io_vertex_normal;

    gl_Position = u_view_projection_matrix * v_model_matrix * vec4(v_position, 1.0f);
}