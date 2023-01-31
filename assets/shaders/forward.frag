#version 460 core

/* IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin. */

// Variables
const int light_point = 0;
const int light_spot = 1;
const int light_directional = 2;

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
    vec3 io_tex_coord;
    vec3 io_vertex_position; // Vertex position in camera space.
    vec3 io_vertex_normal; // Vertex normal in camera space.
    mat3 io_normal_matrix;
    mat3 io_tbn_matrix;
};

// Outputs
layout (location = 0) out vec4 io_frag_colour;

// Uniforms
uniform vec4 u_ambient_colour;
uniform vec4 u_albedo_colour;
uniform float u_gloss;

uniform bool u_texture_albedo_enabled;
uniform bool u_texture_normal_enabled;
uniform bool u_texture_specular_enabled;
uniform bool u_texture_gloss_enabled;
uniform bool u_texture_displacement_enabled;

uniform sampler2D u_texture_albedo;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_gloss;
uniform sampler2D u_texture_displacement;

uniform bool u_enable_lights;
uniform int u_num_lights;
uniform light u_lights[max_lights];

vec4 get_albedo() {
    if (u_texture_albedo_enabled) {
        return texture(u_texture_albedo, io_tex_coord.xy);
    }
    return u_albedo_colour;
}

vec3 get_normal() {
    if (u_texture_normal_enabled) {
        // Get the normal from the texture.
        vec3 normal = texture(u_texture_normal, io_tex_coord.xy).rgb;

        // Since Normals range from -1 to 1, it had to be halved and have 0.5 added to it to convert it to a 0 to 1 range which can be saved as an image.
        // This is to reverse the image value back into the -1 to 1 range.
        normal *= 2.0f;
        normal -= vec3(1.0f, 1.0f, 1.0f);
        normal = normalize(normal);

        // TBN Version
        // Convert the normal from tangent space to camera space.
        normal = normalize(io_tbn_matrix * normal);

        return normal;
    }
    return io_vertex_normal;
}

float get_gloss() {
    return u_texture_gloss_enabled ? texture(u_texture_gloss, io_tex_coord.xy).r : u_gloss;
}

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

    vec3 specular_direction = reflect(vertex_to_light(_light, _vertex_position), _vertex_normal);
    vec3 view_direction = normalize(_vertex_position);

    return pow(max(dot(specular_direction, view_direction), 0.0f), _gloss);
}

vec4 diffuse_colour(const in vec3 _vertex_position, const in vec3 _vertex_normal) {
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

vec4 specular_colour(const in vec3 _vertex_position, const in vec3 _vertex_normal, float _gloss) {
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
        io_frag_colour = get_albedo();
        return;
    }

    float gloss = get_gloss();
    vec3 normal = get_normal();
    io_frag_colour = get_albedo() * (u_ambient_colour + diffuse_colour(io_vertex_position, normal) + specular_colour(io_vertex_position, normal, gloss));
}