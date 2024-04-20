#version 460 core

layout (location = 0) out vec4 out_colour;

// Inputs
in VS_OUT {
    vec2 tex_coord;
} vs_out;

// Constants
const int max_lights = 4;

const int light_point = 0;
const int light_spot = 1;
const int light_directional = 2;

// Transform
uniform mat4 u_inv_view_matrix;

// Textures
uniform sampler2D u_texture_position;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_specular;

// Shadow
uniform sampler2D u_texture_shadows[max_lights];
uniform samplerCube u_cubemap_shadows[max_lights];

// Light
struct light {
    int mode_;
    float power_;
    vec4 colour_;

    float attenuation_constant_;
    float attenuation_linear_;
    float attenuation_quadratic_;

    float spotlight_inner_cosine_;
    float spotlight_outer_cosine_;

    vec3 position_;
    vec3 direction_;

    float shadow_distance_;
    mat4 view_projection_matrix_;
};

uniform int u_num_lights;
uniform vec4 u_ambient_light;
uniform light u_lights[max_lights];

// Includes
bool cast_point_shadow(const in samplerCube _cubemap, const in vec3 _pos, const in vec3 _normal, const in mat4 _inv_view_matrix, const in vec3 _light_pos, float _cast_dist);
bool cast_spot_shadow(const in sampler2D _texture, const in vec3 _pos, const in vec3 _normal, const in mat4 _inv_view_matrix, const in mat4 _light_vp_mat, const in vec3 _light_dir);
bool cast_directional_shadow(const in sampler2D _texture, const in vec3 _pos, const in vec3 _normal, const in mat4 _inv_view_matrix, const in mat4 _light_vp_mat, const in vec3 _light_dir);

float light_attenuation(const in vec3 _pos, const in vec3 _light_pos, const in float _attn_const, const in float _attn_linear, const in float _attn_quad);
float spotlight_effect(const in vec3 _pos, const in vec3 _light_pos, const in vec3 _light_dir, const in float _inner_cos, const in float _outer_cos);
float diffuse_intensity(const in vec3 _pos, const in vec3 _normal, const in vec3 _light_pos, const in vec3 _light_dir, bool _is_dir_light);
float specular_intensity(const in vec3 _pos, const in vec3 _normal, const in vec3 _light_pos, const in vec3 _light_dir, bool _is_dir_light, float _gloss);

// Local Functions
void get_cast_shadow(const in vec3 _position, const in vec3 _normal, inout bool cast_shadow[max_lights]) {
    for (int i = 0; i < u_num_lights; ++i) {
        cast_shadow[i] = (u_lights[i].mode_ == light_point && cast_point_shadow(u_cubemap_shadows[i], _position, _normal, u_inv_view_matrix, u_lights[i].position_, u_lights[i].shadow_distance_)) ||
                         (u_lights[i].mode_ == light_spot && cast_spot_shadow(u_texture_shadows[i], _position, _normal, u_inv_view_matrix, u_lights[i].view_projection_matrix_, u_lights[i].direction_)) ||
                         (u_lights[i].mode_ == light_directional && cast_directional_shadow(u_texture_shadows[i], _position, _normal, u_inv_view_matrix, u_lights[i].view_projection_matrix_, u_lights[i].direction_));
    }
}

vec3 get_light_diffuse(const in vec3 _pos, const in vec3 _normal, const in bool _cast_shadow[max_lights]) {
    vec3 colour = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        if (_cast_shadow[i]) { continue; } // Check if there is shadow.
        switch (u_lights[i].mode_) {
            case light_point:
                colour += diffuse_intensity(_pos, _normal, u_lights[i].position_, u_lights[i].direction_, false) *
                          u_lights[i].colour_.rgb * u_lights[i].power_ *
                          light_attenuation(_pos, u_lights[i].position_, u_lights[i].attenuation_constant_, u_lights[i].attenuation_linear_, u_lights[i].attenuation_quadratic_);
            break;
            case light_spot:
                colour += diffuse_intensity(_pos, _normal, u_lights[i].position_, u_lights[i].direction_, false) *
                          u_lights[i].colour_.rgb * u_lights[i].power_ *
                          light_attenuation(_pos, u_lights[i].position_, u_lights[i].attenuation_constant_, u_lights[i].attenuation_linear_, u_lights[i].attenuation_quadratic_) *
                          spotlight_effect(_pos, u_lights[i].position_, u_lights[i].direction_, u_lights[i].spotlight_inner_cosine_, u_lights[i].spotlight_outer_cosine_);
            break;
            case light_directional:
                colour += diffuse_intensity(_pos, _normal, u_lights[i].position_, u_lights[i].direction_, true) *
                          u_lights[i].colour_.rgb * u_lights[i].power_;
            break;
            default:
            break;
        }
    }
    return colour;
}

vec3 get_light_specular(const in vec3 _pos, const in vec3 _normal, float _gloss, const in bool _cast_shadow[max_lights]) {
    vec3 colour = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        if (_cast_shadow[i]) { continue; } // Check if there is shadow.
        switch (u_lights[i].mode_) {
            case light_point:
                colour += specular_intensity(_pos, _normal, u_lights[i].position_, u_lights[i].direction_, false, _gloss) *
                          u_lights[i].colour_.rgb * u_lights[i].power_ *
                          light_attenuation(_pos, u_lights[i].position_, u_lights[i].attenuation_constant_, u_lights[i].attenuation_linear_, u_lights[i].attenuation_quadratic_);
            break;
            case light_spot:
                colour += specular_intensity(_pos, _normal, u_lights[i].position_, u_lights[i].direction_, false, _gloss) *
                          u_lights[i].colour_.rgb * u_lights[i].power_ *
                          light_attenuation(_pos, u_lights[i].position_, u_lights[i].attenuation_constant_, u_lights[i].attenuation_linear_, u_lights[i].attenuation_quadratic_) *
                          spotlight_effect(_pos, u_lights[i].position_, u_lights[i].direction_, u_lights[i].spotlight_inner_cosine_, u_lights[i].spotlight_outer_cosine_);
            break;
            case light_directional:
                colour += specular_intensity(_pos, _normal, u_lights[i].position_, u_lights[i].direction_, true, _gloss) *
                          u_lights[i].colour_.rgb * u_lights[i].power_;
            break;
            default:
            break;
        }
    }
    return colour;
}

void main() {
    const vec3 pos = texture(u_texture_position, vs_out.tex_coord).rgb;
    const vec3 norm = texture(u_texture_normal, vs_out.tex_coord).rgb;

    const vec4 diff_tex_val = texture(u_texture_diffuse, vs_out.tex_coord);
    const vec3 diff = diff_tex_val.rgb;
    const float alpha = diff_tex_val.a;

    const vec4 spec_tex_val = texture(u_texture_specular, vs_out.tex_coord);
    const vec3 spec = spec_tex_val.rgb;
    const float gloss = spec_tex_val.a;

    bool cast_shadow[max_lights];
    get_cast_shadow(pos, norm, cast_shadow);

    const vec3 ambient = diff * u_ambient_light.rgb;
    const vec3 diffuse = diff * get_light_diffuse(pos, norm, cast_shadow);
    const vec3 specular = spec * get_light_specular(pos, norm, gloss, cast_shadow);
    const vec3 colour = ambient + diffuse + specular;

    out_colour = vec4(colour, alpha);
}