#version 460 core

layout (location = 0) out vec4 io_composite;
layout (location = 1) out vec4 io_diffuse;
layout (location = 2) out vec4 io_specular;

// Variables
const int light_point = 0;
const int light_spot = 1;
const int light_directional = 2;

const int max_lights = 32;

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
    vec3 io_tex_coord;
};

// GPass to LPass
uniform sampler2D u_texture_gbuffer_position;
uniform sampler2D u_texture_gbuffer_normal;
uniform sampler2D u_texture_gbuffer_albedo;
uniform sampler2D u_texture_gbuffer_specular;
uniform sampler2D u_texture_gbuffer_gloss;

// LPass to LPass
uniform sampler2D u_texture_lbuffer_diffuse;
uniform sampler2D u_texture_lbuffer_specular;

// Uniforms
uniform vec4 u_ambient_colour;

uniform bool u_enable_lights;
uniform int u_num_lights;
uniform light u_lights[max_lights];

float light_attenuation(const in light _light, const in vec3 _vertex_position) {
    vec3 direction = _light.position_camera_space_ - _vertex_position;
    float distance_squared = dot(direction, direction);
    float distance = sqrt(distance_squared);
    return _light.power_ / max(1.0f, _light.attenuation_constant_ + (_light.attenuation_linear_ * distance) + (_light.attenuation_quadratic_ * distance_squared));
}

vec3 vertex_to_light(const in light _light, const in vec3 _vertex_position) {
    // Directional Light
    if (_light.mode_ == light_directional) {
        return normalize(-_light.direction_camera_space_);
    }

    // Point Light & Spot Light
    return normalize(_light.position_camera_space_ - _vertex_position);
}

float spotlight_effect(const in light _light, const in vec3 _vertex_position) {
    vec3 light_to_vertex = normalize(_vertex_position - _light.position_camera_space_);
    float vertex_angle_cosine = dot(light_to_vertex, _light.direction_camera_space_);

    // As long as the angle between the light direction and light_to_vertex is within the inner cosine, the light is at full power. The light power will falloff as it approaches the outer cosine.
    return clamp((vertex_angle_cosine - _light.spotlight_outer_cosine_) / (_light.spotlight_inner_cosine_ - _light.spotlight_outer_cosine_), 0.0f, 1.0f);
}

float diffuse_intensity(const in light _light, const in vec3 _vertex_position, const in vec3 _vertex_normal) {
    return clamp(dot(vertex_to_light(_light, _vertex_position), _vertex_normal), 0.0f, 1.0f);
}

float specular_intensity(const in light _light, const in vec3 _vertex_position, const in vec3 _vertex_normal, float _gloss) {
    vec3 specular_direction = reflect(vertex_to_light(_light, _vertex_position), _vertex_normal);
    vec3 view_direction = normalize(_vertex_position);
    return pow(max(dot(specular_direction, view_direction), 0.0f), _gloss);
}

vec4 light_diffuse(const in vec3 _vertex_position, const in vec3 _vertex_normal) {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        switch (u_lights[i].mode_) {
            case light_point:
            colour += u_lights[i].colour_ *
            diffuse_intensity(u_lights[i], _vertex_position, _vertex_normal) *
            light_attenuation(u_lights[i], _vertex_position);
            break;
            case light_spot:
            colour += u_lights[i].colour_ *
            diffuse_intensity(u_lights[i], _vertex_position, _vertex_normal) *
            light_attenuation(u_lights[i], _vertex_position) *
            spotlight_effect(u_lights[i], _vertex_position);
            break;
            case light_directional:
            colour += u_lights[i].colour_ *
            diffuse_intensity(u_lights[i], _vertex_position, _vertex_normal) *
            u_lights[i].power_;
            break;
        }
    }
    return colour;
}

vec4 light_specular(const in vec3 _vertex_position, const in vec3 _vertex_normal, float _gloss) {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        switch (u_lights[i].mode_) {
            case light_point:
            colour +=
                u_lights[i].colour_ *
                specular_intensity(u_lights[i], _vertex_position, _vertex_normal, _gloss) *
                light_attenuation(u_lights[i], _vertex_position);
            break;
            case light_spot:
            colour +=
                u_lights[i].colour_ *
                specular_intensity(u_lights[i], _vertex_position, _vertex_normal, _gloss) *
                light_attenuation(u_lights[i], _vertex_position) *
                spotlight_effect(u_lights[i], _vertex_position);
            break;
            case light_directional:
            colour +=
                u_lights[i].colour_ *
                specular_intensity(u_lights[i], _vertex_position, _vertex_normal, _gloss) *
                u_lights[i].power_;
            break;
        }
    }
    return colour;
}

void main() {
    if (!u_enable_lights) {
        io_composite = texture(u_texture_gbuffer_albedo, io_tex_coord.xy);
        io_diffuse = texture(u_texture_lbuffer_diffuse, io_tex_coord.xy);
        io_specular = texture(u_texture_lbuffer_specular, io_tex_coord.xy);
        return;
    }

    const vec3 position = texture(u_texture_gbuffer_position, io_tex_coord.xy).rgb;
    const vec3 normal = texture(u_texture_gbuffer_normal, io_tex_coord.xy).rgb;
    const vec4 albedo = texture(u_texture_gbuffer_albedo, io_tex_coord.xy);
    const vec4 specular = texture(u_texture_gbuffer_specular, io_tex_coord.xy);
    const float gloss = texture(u_texture_gbuffer_gloss, io_tex_coord.xy).r;

    const vec4 ambient_colour = albedo * u_ambient_colour;
    const vec4 diffuse_colour = albedo * light_diffuse(position, normal) + texture(u_texture_lbuffer_diffuse, io_tex_coord.xy);
    const vec4 specular_colour = specular * light_specular(position, normal, gloss) + texture(u_texture_lbuffer_specular, io_tex_coord.xy);

    io_composite = vec4((ambient_colour + diffuse_colour + specular_colour).rgb, 1.0f);
    io_diffuse = diffuse_colour;
    io_specular = specular_colour;
}