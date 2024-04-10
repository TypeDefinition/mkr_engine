#version 460 core

// Inputs
in GS_OUT {
    vec2 tex_coord;
    vec4 frag_pos;
} gs_out;

uniform vec3 u_light_pos;
uniform float u_shadow_distance;

uniform bool is_transparent;
uniform vec4 u_diffuse_colour;
uniform bool u_texture_diffuse_enabled;
uniform sampler2D u_texture_diffuse;

void main() {
    if (is_transparent) {
        // Discard transparent fragments.
        float alpha = u_diffuse_colour.a;
        if (u_texture_diffuse_enabled) {
            alpha *= texture(u_texture_diffuse, gs_out.tex_coord).a;
        }
        if (alpha < 0.9f) { discard; }
    }

    // Get distance between fragment and light source.
    float distance = length(gs_out.frag_pos.xyz - u_light_pos);
    // Map to [0, 1] range by dividing by shadow distance.
    gl_FragDepth = distance / u_shadow_distance;
}