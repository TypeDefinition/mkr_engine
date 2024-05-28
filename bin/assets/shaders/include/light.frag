// Constants
const int max_lights = 4;

const int light_point = 0;
const int light_spot = 1;
const int light_directional = 2;

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

// Uniforms
uniform int u_num_lights;
uniform vec4 u_ambient_light;
uniform light u_lights[max_lights];

float light_attenuation(const in vec3 _pos,
                        const in vec3 _light_pos,
                        const in float _attn_const, const in float _attn_linear, const in float _attn_quad) {
    const vec3 dir = _light_pos - _pos;
    const float dist_sqr = dot(dir, dir);
    const float dist = sqrt(dist_sqr);
    return 1.0f / max(1.0f, _attn_const + (_attn_linear * dist) + (_attn_quad * dist_sqr));
}

float spotlight_effect(const in vec3 _pos,
                       const in vec3 _light_pos, const in vec3 _light_dir,
                       const in float _inner_cos, const in float _outer_cos) {
    const vec3 light_to_frag = normalize(_pos - _light_pos);
    const vec3 spot_direction = normalize(_light_dir);
    const float cosine_angle = dot(light_to_frag, spot_direction);// A·B = |A||B|cos(θ), where |A| = |B| = 1.

    // As long as the angle between the light direction and light_direction is within the inner cosine, the light is at full power. The light power will falloff as it approaches the outer cosine.
    return clamp((cosine_angle - _outer_cos) / (_inner_cos - _outer_cos), 0.0f, 1.0f);
}

float diffuse_intensity(const in vec3 _pos, const in vec3 _normal,
                        const in vec3 _light_pos, const in vec3 _light_dir, bool _is_dir_light) {
    const vec3 frag_to_light = _is_dir_light ? normalize(-_light_dir) : normalize(_light_pos - _pos);
    return clamp(dot(frag_to_light, _normal), 0.0f, 1.0f);
}

float specular_intensity(const in vec3 _pos, const in vec3 _normal,
                         const in vec3 _light_pos, const in vec3 _light_dir, bool _is_dir_light,
                         float _gloss) {
    const vec3 light_to_frag = _is_dir_light ? normalize(_light_dir) : normalize(_pos - _light_pos);
    const vec3 view_dir = normalize(-_pos);
    return pow(max(dot(reflect(light_to_frag, _normal), view_dir), 0.0f), 32.0f * _gloss);
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