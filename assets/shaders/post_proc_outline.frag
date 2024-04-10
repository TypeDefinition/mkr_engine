#version 460 core

layout (location = 0) out vec4 out_composite;

// Inputs
in VS_OUT {
    vec2 tex_coord;
} vs_out;

uniform sampler2D u_texture_frag_position;
uniform sampler2D u_texture_frag_normal;
uniform sampler2D u_texture_depth_stencil;
uniform sampler2D u_texture_composite;

uniform float u_near;
uniform float u_far;
uniform vec2 u_bottom_left;
uniform vec2 u_top_right;

// Custom Uniform(s)
uniform float u_depth_threshold = 0.1f;
uniform float u_normal_threshold = 1.0f;
uniform vec4 u_line_colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);

// Local Constant(s)
const float kernal_vertical[9] = float[](1.0f, 0.0f, -1.0f, 2.0f, 0.0f, -2.0f, 1.0f, 0.0f, -1.0f); // Sobel Edge Detection Kernal
const float kernal_horizontal[9] = float[](1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -2.0f, -1.0f); // Sobel Edge Detection Kernal

// We don't need this function now, but this is just a good to know. Convert From -1 to 1 Range to 0 to 1 Range. Opposite of linearised_depth but without the conversion to the u_near to u_far range.
float normalised_depth(float _depth) {
    return (_depth - u_near) / (u_far - u_near);
}

float linearised_depth(float _depth) {
    float ndc_depth = _depth * 2.0f -1.0f;// Convert Depth to NDC space. (From 0 to 1 Range to -1 to 1 Range)
    return (2.0 * u_near) / (u_far + u_near - ndc_depth * (u_far - u_near));// Convert the depth to the u_near to u_far range.
}

bool depth_check() {
    float delta_vertical = 0.0f;
    float delta_horizontal = 0.0f;
    float depth = linearised_depth(texelFetch(u_texture_depth_stencil, ivec2(gl_FragCoord), 0).r);// To get the depth from the Depth24_Stencil8 Buffer, use r or x.

    int index = 0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float neighbour_depth  = linearised_depth(texelFetch(u_texture_depth_stencil, ivec2(gl_FragCoord) + ivec2(x, y), 0).r);
            delta_vertical += (neighbour_depth - depth) * kernal_vertical[index];
            delta_horizontal += (neighbour_depth - depth) * kernal_horizontal[index];
            ++index;
        }
    }

    float gradient_squared = (delta_vertical * delta_vertical) + (delta_horizontal * delta_horizontal);
    return gradient_squared > (u_depth_threshold * u_depth_threshold);
}

bool normal_check() {
    float delta_vertical = 0.0f;
    float delta_horizontal = 0.0f;
    vec3 normal = texelFetch(u_texture_frag_normal, ivec2(gl_FragCoord), 0).rgb;

    int index = 0;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec3 neighbour_normal  = texelFetch(u_texture_frag_normal, ivec2(gl_FragCoord) + ivec2(x, y), 0).rgb;
            delta_vertical += dot(normal, neighbour_normal) * kernal_vertical[index];
            delta_horizontal += dot(normal, neighbour_normal) * kernal_horizontal[index];
            ++index;
        }
    }

    float gradient_squared = (delta_vertical * delta_vertical) + (delta_horizontal * delta_horizontal);
    return gradient_squared > (u_normal_threshold * u_normal_threshold);
}

void main() {
    if ((u_bottom_left.x + 1) < gl_FragCoord.x && gl_FragCoord.x < (u_top_right.x - 1) &&
        (u_bottom_left.y + 1) < gl_FragCoord.y && gl_FragCoord.y < (u_top_right.y - 1) &&
        (depth_check() || normal_check())) {
        out_composite = u_line_colour;
    } else {
        out_composite = texelFetch(u_texture_composite, ivec2(gl_FragCoord), 0);
    }
}