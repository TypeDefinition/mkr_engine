#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in VS_OUT {
    vec2 tex_coord;
} vs_out[];

out GS_OUT {
    vec2 tex_coord;
    vec4 frag_pos;
} gs_out;

// Uniforms
uniform mat4 u_view_projection_matrices[6];

void main() {
    for (int i = 0; i < 6; ++i) {
        gl_Layer = i;// Built-in variable that specifies to which face we render.
        for (int j = 0; j < 3; ++j) { // For each triangle vertex.
            gs_out.tex_coord = vs_out[j].tex_coord;
            gs_out.frag_pos = gl_in[j].gl_Position;
            gl_Position = u_view_projection_matrices[i] * gs_out.frag_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}