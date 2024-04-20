#version 460 core

/* IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin. */

// An alternative to the layout qualifier is to use glBindFragDataLocation(GLuint program, GLuint colourNumber, const char * name) or
// glBindFragDataLocationIndexed(GLuint program, GLuint colourNumber, GLuint index, const char *name) in the C++ code.
// By using glBindFragDataLocation(GLuint program, GLuint colourNumber, const char * name), it is not necessary to use the layout qualifier and vice-versa.
// However, if both are used and are in conflict, the layout qualifier in the vertex shader has priority.
layout (location = 0) out vec4 out_accumulation;
layout (location = 1) out float out_revealage;

// Inputs
in VS_OUT {
    vec2 tex_coord;
    vec3 position;// Vertex position in camera space.
    vec3 normal;// Vertex normal in camera space.
    mat3 tbn_matrix;// Converts from tangent space to camera space.
} vs_out;

// Constants
const int max_lights = 4;

const int light_point = 0;
const int light_spot = 1;
const int light_directional = 2;

// Transform
uniform mat4 u_view_matrix;
uniform mat4 u_inv_view_matrix;

// Material
uniform vec4 u_diffuse_colour;
uniform vec4 u_specular_colour;
uniform float u_displacement_scale;

// Textures
uniform bool u_texture_diffuse_enabled;
uniform bool u_texture_normal_enabled;
uniform bool u_texture_specular_enabled;
uniform bool u_texture_displacement_enabled;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_displacement;

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
vec2 parallax_occlusion(const in sampler2D _texture, const in vec2 _tex_coord, float _disp_scale, const in vec3 _frag_pos, const in mat3 _inv_tbn_matrix, const in vec3 _normal);

bool cast_point_shadow(const in samplerCube _cubemap, const in vec3 _pos, const in vec3 _normal, const in mat4 _inv_view_matrix, const in vec3 _light_pos, float _cast_dist);
bool cast_spot_shadow(const in sampler2D _texture, const in vec3 _pos, const in vec3 _normal, const in mat4 _inv_view_matrix, const in mat4 _light_vp_mat, const in vec3 _light_dir);
bool cast_directional_shadow(const in sampler2D _texture, const in vec3 _pos, const in vec3 _normal, const in mat4 _inv_view_matrix, const in mat4 _light_vp_mat, const in vec3 _light_dir);

float light_attenuation(const in vec3 _pos, const in vec3 _light_pos, const in float _attn_const, const in float _attn_linear, const in float _attn_quad);
float spotlight_effect(const in vec3 _pos, const in vec3 _light_pos, const in vec3 _light_dir, const in float _inner_cos, const in float _outer_cos);
float diffuse_intensity(const in vec3 _pos, const in vec3 _normal, const in vec3 _light_pos, const in vec3 _light_dir, bool _is_dir_light);
float specular_intensity(const in vec3 _pos, const in vec3 _normal, const in vec3 _light_pos, const in vec3 _light_dir, bool _is_dir_light, float _gloss);

// Local Functions
void get_cast_shadow(const in vec3 _normal, inout bool cast_shadow[max_lights]) {
    for (int i = 0; i < u_num_lights; ++i) {
        cast_shadow[i] = (u_lights[i].mode_ == light_point && cast_point_shadow(u_cubemap_shadows[i], vs_out.position, _normal, u_inv_view_matrix, u_lights[i].position_, u_lights[i].shadow_distance_)) ||
                         (u_lights[i].mode_ == light_spot && cast_spot_shadow(u_texture_shadows[i], vs_out.position, _normal, u_inv_view_matrix, u_lights[i].view_projection_matrix_, u_lights[i].direction_)) ||
                         (u_lights[i].mode_ == light_directional && cast_directional_shadow(u_texture_shadows[i], vs_out.position, _normal, u_inv_view_matrix, u_lights[i].view_projection_matrix_, u_lights[i].direction_));
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

vec2 get_tex_coord() {
    return u_texture_displacement_enabled ? parallax_occlusion(u_texture_displacement, vs_out.tex_coord, u_displacement_scale, vs_out.position, vs_out.tbn_matrix, vs_out.normal) : vs_out.tex_coord;
}

vec3 get_normal(const in vec2 _tex_coord) {
    if (u_texture_normal_enabled) {
        const vec3 normal = 2.0f * texture(u_texture_normal, _tex_coord).rgb - vec3(1.0f, 1.0f, 1.0f); // Convert into the [-1, 1] range.
        return vs_out.tbn_matrix * normal; // Convert the normal from tangent space to camera space.
    }
    return vs_out.normal;
}

void get_diffuse(const in vec2 _tex_coord, inout vec3 _colour, inout float _alpha) {
    _colour = u_diffuse_colour.rgb;
    _alpha = u_diffuse_colour.a;
    if (u_texture_diffuse_enabled) {
        vec4 tex_val = texture(u_texture_diffuse, _tex_coord);
        _colour *= tex_val.rgb;
        _alpha *= tex_val.a;
    }
}

void get_specular(const in vec2 _tex_coord, inout vec3 _colour, inout float _gloss) {
    _colour = u_specular_colour.rgb;
    _gloss = u_specular_colour.a;
    if (u_texture_specular_enabled) {
        vec4 tex_val = texture(u_texture_specular, _tex_coord);
        _colour *= tex_val.rgb;
        _gloss *= tex_val.a;
    }
}

void main() {
    const vec2 tex_coord = get_tex_coord();
    const vec3 normal = get_normal(tex_coord);

    bool cast_shadow[max_lights];
    get_cast_shadow(normal, cast_shadow);

    vec3 diffuse; float alpha;
    get_diffuse(tex_coord, diffuse, alpha);
    vec3 specular; float gloss;
    get_specular(tex_coord, specular, gloss);

    const vec3 ambient = diffuse * u_ambient_light.rgb;
    diffuse *= get_light_diffuse(vs_out.position, normal, cast_shadow);
    specular *= get_light_specular(vs_out.position, normal, gloss, cast_shadow);

    const vec3 colour = ambient + diffuse + specular;

    /* Random weight function I ripped off from Morgan McGuire (inventor of the weighted-blended OIT technique).
       No, I do not understand what it means either. */
    const float weight = max(min(1.0f, max(max(colour.r, colour.g), colour.b) * alpha), alpha) * clamp(0.03f / (0.00001f + pow(gl_FragCoord.z / 200.0f, 4.0f)), 0.01f, 3000.0f);

    out_accumulation = vec4(colour * alpha, alpha) * weight;
    out_revealage = alpha;
}