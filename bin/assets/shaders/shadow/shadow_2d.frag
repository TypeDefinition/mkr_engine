#version 460 core

// Inputs
in VS_OUT {
    vec2 tex_coord;
} vs_out;

// Uniforms
uniform bool u_is_transparent;
uniform vec4 u_diffuse_colour;
uniform bool u_has_texture_diffuse;
uniform sampler2D u_texture_diffuse;

void main() {
    if (u_is_transparent) {
        // Discard transparent fragments.
        float alpha = u_diffuse_colour.a;
        if (u_has_texture_diffuse) {
            alpha *= texture(u_texture_diffuse, vs_out.tex_coord).a;
        }
        if (alpha < 0.9f) { discard; }
    }
}