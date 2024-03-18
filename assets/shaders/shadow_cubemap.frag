#version 460 core

in vec4 frag_pos;

uniform vec3 u_light_pos;
uniform float u_shadow_distance;

void main() {
    // Get distance between fragment and light source.
    float distance = length(frag_pos.xyz - u_light_pos);
    // Map to [0, 1] range by dividing by shadow distance.
    gl_FragDepth = distance / u_shadow_distance;
}