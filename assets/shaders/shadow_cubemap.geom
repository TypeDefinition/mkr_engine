#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

out vec4 frag_pos; // World space fragment position. (Output per EmitVertex().)

// Uniforms
uniform mat4 u_view_projection_matrices[6];

void main() {
    for (int i = 0; i < 6; ++i) {
        gl_Layer = i;// Built-in variable that specifies to which face we render.
        for (int j = 0; j < 3; ++j) { // For each triangle vertex.
            frag_pos = gl_in[j].gl_Position;
            gl_Position = u_view_projection_matrices[i] * frag_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}