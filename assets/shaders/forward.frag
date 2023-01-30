#version 460 core

// Variables
const int light_mode_point = 0;
const int light_mode_spot = 1;
const int light_mode_directional = 2;

const int max_lights = 64;

struct light {
    int mode_;
    float power_;
    vec4 colour_;

    float attenuation_constant_;
    float attenuation_linear_;
    float attenuation_quadratic_;

    float spotlight_inner_cosine_;
    float spotlight_outer_cosine_;

    vec3 position_camera_space_;
    vec3 direction_camera_space_;
};

// Inputs
in io_block {
    vec3 io_position_camera_space;
    vec3 io_normal_camera_space;
    vec3 io_tex_coord;
};

// Outputs
layout (location = 0) out vec4 io_frag_colour;

// Uniforms
// Lights
uniform vec4 u_ambient_colour;
uniform int u_num_lights;
uniform light u_lights[max_lights];

// Textures
uniform sampler2D u_texture_albedo;

float spotlight_effect(int _light_index, vec3 _vertex_position_camera_space) {
    vec3 light_to_vertex = normalize(_vertex_position_camera_space - u_lights[_light_index].position_camera_space_);
    float vertex_angle_cosine = dot(light_to_vertex, u_lights[_light_index].direction_camera_space_);

    // As long as the angle between the light direction and light_to_vertex is within the inner cosine, the light is at full power. The light power will falloff as it approaches the outer cosine.
    return clamp((vertex_angle_cosine - u_lights[_light_index].spotlight_outer_cosine_) / (u_lights[_light_index].spotlight_inner_cosine_ - u_lights[_light_index].spotlight_outer_cosine_), 0.0f, 1.0f);
}

float light_attenuation(int _light_index, vec3 _vertex_position_camera_space) {
    float distance = length(u_lights[_light_index].position_camera_space_ - _vertex_position_camera_space);
    return u_lights[_light_index].power_ / max(1.0f, u_lights[_light_index].attenuation_constant_ + (u_lights[_light_index].attenuation_linear_ * distance) + (u_lights[_light_index].attenuation_quadratic_ * distance * distance));
}

vec3 vertex_to_light(int _light_index, vec3 _vertex_position_camera_space) {
    // Directional Light
    if (u_lights[_light_index].mode_ == light_mode_directional) {
        return normalize(-u_lights[_light_index].direction_camera_space_);
    }

    // Point Light & Spot Light
    return normalize(u_lights[_light_index].position_camera_space_ - _vertex_position_camera_space);
}

float diffuse_intensity(int _light_index, vec3 _vertex_position_camera_space, vec3 _vertex_normal_camera_space) {
    return clamp(dot(vertex_to_light(_light_index, _vertex_position_camera_space), _vertex_normal_camera_space), 0.0f, 1.0f);
}

vec4 diffuse_colour(vec3 _vertex_position_camera_space, vec3 _vertex_normal_camera_space) {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        switch (u_lights[i].mode_) {
            case light_mode_point:
            colour += u_lights[i].colour_ *
                diffuse_intensity(i, _vertex_position_camera_space, _vertex_normal_camera_space) *
                light_attenuation(i, _vertex_position_camera_space);
            break;
            case light_mode_spot:
            colour += u_lights[i].colour_ *
                diffuse_intensity(i, _vertex_position_camera_space, _vertex_normal_camera_space) *
                light_attenuation(i, _vertex_position_camera_space) * spotlight_effect(i, _vertex_position_camera_space);
            break;
            case light_mode_directional:
            colour += u_lights[i].colour_ *
                diffuse_intensity(i, _vertex_position_camera_space, _vertex_normal_camera_space) *
                u_lights[i].power_;
            break;
        }
    }
    return colour;
}

float specular_intensity(int _light_index, vec3 _vertex_position_camera_space, vec3 _vertex_normal_camera_space, float _gloss) {
    /*
    To get out specular intensity, we need the dot product of the reflection of the light_to_vertex vector and the vertex_to_camera vector.

    The reflect function expects the first vector to point from the light source towards the fragment's position,
    but vertex_to_light is currently pointing the other way around: from the fragment towards the light source.
    To make sure we get the correct reflect vector we reverse its direction by negating vertex_to_light.
    The second argument expects a normal vector so we supply the normalized norm vector.

    But instead of doing that, the other way is to simply leave vertex_to_light the way it is, and dot product it
    with the camera_to_vertex vector instead. To understand why this works, the reflect(genType I, genType N) function
    works by returning the result of I - 2*dot(N, I) * N. Draw this on a piece of paper with a vector and a normal and see how it reflects.
    */

    vec3 specular_direction = reflect(vertex_to_light(_light_index, _vertex_position_camera_space), _vertex_normal_camera_space);
    vec3 view_direction = normalize(_vertex_position_camera_space);

    return pow(max(dot(specular_direction, view_direction), 0.0f), _gloss);
}

vec4 specular_colour(vec3 _vertex_position_camera_space, vec3 _vertex_normal_camera_space, float _gloss) {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        switch (u_lights[i].mode_) {
            case light_mode_point:
            colour +=
                specular_intensity(i, _vertex_position_camera_space, _vertex_normal_camera_space, _gloss) *
                light_attenuation(i, _vertex_position_camera_space) * u_lights[i].colour_;
            break;
            case light_mode_spot:
            colour +=
                specular_intensity(i, _vertex_position_camera_space, _vertex_normal_camera_space, _gloss) *
                light_attenuation(i, _vertex_position_camera_space) *
                spotlight_effect(i, _vertex_position_camera_space) * u_lights[i].colour_;
            break;
            case light_mode_directional:
            colour +=
                specular_intensity(i, _vertex_position_camera_space, _vertex_normal_camera_space, _gloss) *
                u_lights[i].power_ * u_lights[i].colour_;
            break;
        }
    }
    return colour;
}

void main() {
    io_frag_colour = (u_ambient_colour + diffuse_colour(io_position_camera_space, io_normal_camera_space) + specular_colour(io_position_camera_space, io_normal_camera_space, 0.3f)) * texture(u_texture_albedo, io_tex_coord.xy);
}