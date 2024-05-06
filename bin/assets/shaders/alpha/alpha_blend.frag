#version 460 core

layout (location = 0) out vec4 out_colour;

in VS_OUT {
    vec4 frag_pos;
} vs_out;

uniform sampler2D u_texture_accumulation;
uniform sampler2D u_texture_revealage;

const float epsilon = 0.00001f;

void main() {
    const vec2 tex_coord = (vs_out.frag_pos.xy / vs_out.frag_pos.w) * 0.5f + vec2(0.5f, 0.5f);
    const vec4 accum = texture(u_texture_accumulation, tex_coord);
    const float reveal = texture(u_texture_revealage, tex_coord).r;

    out_colour = vec4(accum.rgb / max(accum.a, epsilon), reveal);
}