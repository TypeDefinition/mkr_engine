#version 460 core

/* IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin. */

// An alternative to the layout qualifier is to use glBindFragDataLocation(GLuint program, GLuint colorNumber, const char * name) or
// glBindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const char *name) in the C++ code.
// By using glBindFragDataLocation(GLuint program, GLuint colorNumber, const char * name), it is not necessary to use the layout qualifier and vice-versa.
// However, if both are used and are in conflict, the layout qualifier in the vertex shader has priority.
layout (location = 0) out vec4 out_colour;
layout (location = 1) out vec4 out_position;
layout (location = 2) out vec4 out_normal;

// Inputs
in io_block {
    vec3 io_tex_coord;
    vec3 io_position;// Vertex position in camera space.
    vec3 io_normal;// Vertex normal in camera space.
    mat3 io_tbn_matrix;// Converts from tangent space to camera space.
    mat3 io_inv_tbn_matrix;// Converts from camera space to tangent space.
};

// Constants
const int light_point = 0;
const int light_spot = 1;
const int light_directional = 2;
const int max_lights = 4;

// Transform
uniform mat4 u_view_matrix;
uniform mat4 u_inv_view_matrix;

// Material
uniform vec4 u_diffuse_colour;
uniform vec4 u_specular_colour;
uniform float u_gloss;
uniform float u_displacement_scale;

// Texture
uniform bool u_texture_diffuse_enabled;
uniform bool u_texture_normal_enabled;
uniform bool u_texture_specular_enabled;
uniform bool u_texture_gloss_enabled;
uniform bool u_texture_displacement_enabled;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_gloss;
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

// Variables
bool g_cast_shadow[max_lights];

vec2 parallax(const vec2 _tex_coord) {
    // Get the direction of the camera to the fragment's surface in tangent space.
    vec3 view_direction = normalize(io_inv_tbn_matrix * io_position);
    // Convert height map to depth map because depth looks nicer then height.
    // A height of 0.9 is a depth of 0.1, a height of 0.4 is a depth of 0.6 etc.
    float depth = 1.0f - texture(u_texture_displacement, _tex_coord).r;

    // Note the division of view_direction.xy by view_direction.z.
    // As the view_direction vector is normalized view_direction.z will be somewhere in the range between 0.0 and 1.0.
    // When view_direction is largely parallel to the surface its z component is close to 0.0 and the division returns a much larger offset as
    // compared to when view_direction is perpendicular to the surface.
    // So basically we're increasing the size of the offset in such a way that it offsets the texture coordinates at a larger scale when
    // looking at a surface from a steeper angle.

    // Some people prefer to leave the division by view_direction.z out of the equation as normal Parallax Mapping could produce undesirable results at certain angles.
    // This is called "Parallax Mapping with Offset Limiting".
    // Choosing which technique to pick is usually a matter of personal preference.

    // Do take note that if we divide by view_direction.z, make sure to negate it,
    // as we are in tangent space, view_direction.z is a negative number, and any division by it will negate our result.
    return (view_direction.xy / -view_direction.z) * depth * u_displacement_scale;// Parallax Mapping without Offset Limiting
}

vec2 parallax_occlusion(const vec2 _tex_coord) {
    // Parallax Occlusion Mapping is based on the same principles as Steep Parallax Mapping
    // but instead of taking the texture coordinates of the first depth layer after a collision,
    // we're going to linearly interpolate between the depth layer after and before the collision.
    // We base the weight of the linear interpolation on how far the surface's height is from the depth layer's value of both layers.
    // This is again an approximation, but significantly more accurate than Steep Parallax Mapping.

    // Get the UV displacement when the depth is 1. This is the maximum possible UV displacement we can have.
    const vec2 max_uv_displacement = parallax(_tex_coord);

    // Rather than take a fixed number of samples each time, we are going to make use of the
    // fact that only from the sides do we need a higher number of samples.
    // When view perpendicularly, we do not need as many samples as there is no need to displace
    // anything when a fragment is viewed directly perpendicular.
    // When the surface is viewed perpendicularly, the dot product betweem the surface normal and
    // fragment to camera vector is 1, and when viewed sideways, the dot product is 0.
    // By doing this, we can select how many samples we actually want to calculate.
    const int min_displacement_samples = 8;
    const int max_displacement_samples = 16;
    const float dot_product = dot(io_normal, normalize(-io_position));
    const int num_samples = int(mix(max_displacement_samples, min_displacement_samples, dot_product));

    float previous_layer_depth = 0.0f;
    vec2 previous_uv_displacement = vec2(0.0f, 0.0f);
    float previous_texture_depth = 0.0f;
    for (int i = 0; i < num_samples; ++i) {
        const float current_layer_depth = float(i)/float(num_samples);
        const vec2 current_uv_displacement = current_layer_depth * max_uv_displacement;
        const float current_texture_depth = 1.0f - texture(u_texture_displacement, _tex_coord + current_uv_displacement).r;

        // Interpolate between previous_uv_displacement and current_uv_displacement.
        if (current_layer_depth > current_texture_depth) {
            // Currently we have overshot by this much.
            const float excess_depth = current_layer_depth - current_texture_depth;
            // Previously we undershot by this much.
            const float lacking_depth = previous_texture_depth - previous_layer_depth;
            return mix(previous_uv_displacement, current_uv_displacement, lacking_depth / (lacking_depth + excess_depth));
        }

        previous_layer_depth = current_layer_depth;
        previous_uv_displacement = current_uv_displacement;
        previous_texture_depth = current_texture_depth;
    }

    return max_uv_displacement;
}

float light_attenuation(const in light _light, const in vec3 _frag_position) {
    vec3 direction = _light.position_ - _frag_position;
    float distance_squared = dot(direction, direction);
    float distance = sqrt(distance_squared);
    return _light.power_ / max(1.0f, _light.attenuation_constant_ + (_light.attenuation_linear_ * distance) + (_light.attenuation_quadratic_ * distance_squared));
}

float spotlight_effect(const in light _light, const in vec3 _frag_position) {
    vec3 light_to_frag = normalize(_frag_position - _light.position_);
    vec3 spot_direction = normalize(_light.direction_);
    float cosine_angle = dot(light_to_frag, spot_direction);// A·B = |A||B|cos(θ), where |A| = |B| = 1.

    // As long as the angle between the light direction and light_direction is within the inner cosine, the light is at full power. The light power will falloff as it approaches the outer cosine.
    return clamp((cosine_angle - _light.spotlight_outer_cosine_) / (_light.spotlight_inner_cosine_ - _light.spotlight_outer_cosine_), 0.0f, 1.0f);
}

float diffuse_intensity(const in light _light, const in vec3 _frag_position, const in vec3 _frag_normal) {
    vec3 frag_to_light = _light.mode_ == light_directional ? normalize(-_light.direction_) : normalize(_light.position_ - _frag_position);
    return clamp(dot(frag_to_light, _frag_normal), 0.0f, 1.0f);
}

float specular_intensity(const in light _light, const in vec3 _frag_position, const in vec3 _frag_normal, float _gloss) {
    vec3 light_to_frag = _light.mode_ == light_directional ? normalize(_light.direction_) : normalize(_frag_position - _light.position_);
    vec3 view_direction = normalize(-_frag_position);
    return pow(max(dot(reflect(light_to_frag, _frag_normal), view_direction), 0.0f), _gloss);
}

vec4 light_diffuse(const in vec3 _frag_position, const in vec3 _frag_normal) {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        if (g_cast_shadow[i]) { continue; } // Check if there is shadow.
        switch (u_lights[i].mode_) {
            case light_point:
            colour +=
                diffuse_intensity(u_lights[i], _frag_position, _frag_normal) *
                u_lights[i].colour_ * u_lights[i].power_ *
                light_attenuation(u_lights[i], _frag_position);
            break;
            case light_spot:
            colour +=
                diffuse_intensity(u_lights[i], _frag_position, _frag_normal) *
                u_lights[i].colour_ * u_lights[i].power_ *
                light_attenuation(u_lights[i], _frag_position) *
                spotlight_effect(u_lights[i], _frag_position);
            break;
            case light_directional:
            colour +=
                diffuse_intensity(u_lights[i], _frag_position, _frag_normal) *
                u_lights[i].colour_ * u_lights[i].power_;
            break;
            default:
            break;
        }
    }
    return colour;
}

vec4 light_specular(const in vec3 _frag_position, const in vec3 _frag_normal, float _gloss) {
    vec4 colour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    for (int i = 0; i < u_num_lights; ++i) {
        if (g_cast_shadow[i]) { continue; } // Check if there is shadow.
        switch (u_lights[i].mode_) {
            case light_point:
            colour +=
                specular_intensity(u_lights[i], _frag_position, _frag_normal, _gloss) *
                u_lights[i].colour_ * u_lights[i].power_ *
                light_attenuation(u_lights[i], _frag_position);
            break;
            case light_spot:
            colour +=
                specular_intensity(u_lights[i], _frag_position, _frag_normal, _gloss) *
                u_lights[i].colour_ * u_lights[i].power_ *
                light_attenuation(u_lights[i], _frag_position) *
                spotlight_effect(u_lights[i], _frag_position);
            break;
            case light_directional:
            colour +=
                specular_intensity(u_lights[i], _frag_position, _frag_normal, _gloss) *
                u_lights[i].colour_ * u_lights[i].power_;
            break;
            default:
            break;
        }
    }
    return colour;
}

vec2 get_tex_coord() {
    return u_texture_displacement_enabled ? (io_tex_coord.xy + parallax_occlusion(io_tex_coord.xy)) : io_tex_coord.xy;
}

vec3 get_normal(const in vec2 _tex_coord) {
    if (u_texture_normal_enabled) {
        // Get the normal from the texture.
        vec3 normal = texture(u_texture_normal, _tex_coord).rgb;

        // Since Normals range from -1 to 1, it had to be halved and have 0.5 added to it to convert it to a 0 to 1 range which can be saved as an image.
        // This is to reverse the image value back into the -1 to 1 range.
        normal *= 2.0f;
        normal -= vec3(1.0f, 1.0f, 1.0f);
        normal = normalize(normal);

        // TBN Version
        // Convert the normal from tangent space to camera space.
        return io_tbn_matrix * normal;
    }
    return io_normal;
}

vec4 get_diffuse(const in vec2 _tex_coord) {
    return u_texture_diffuse_enabled ? texture(u_texture_diffuse, _tex_coord) * u_diffuse_colour : u_diffuse_colour;
}

vec4 get_specular(const in vec2 _tex_coord) {
    return u_texture_specular_enabled ? texture(u_texture_specular, _tex_coord) * u_specular_colour : u_specular_colour;
}

float get_gloss(const in vec2 _tex_coord) {
    return u_texture_gloss_enabled ? texture(u_texture_gloss, _tex_coord).r * u_gloss : u_gloss;
}

bool cast_point_shadow(const in light _light, const in samplerCube _cubemap, const in vec3 _frag_normal, float _bias) {
    const vec4 frag_world_pos =  u_inv_view_matrix * vec4(io_position, 1.0f); // World space.
    const vec4 light_world_pos =  u_inv_view_matrix * vec4(_light.position_, 1.0f); // World space.
    const vec3 tex_coord = (frag_world_pos - light_world_pos).xyz; // No need to normalise the tex_coord for a cubemap.

    // Distance from light to shadow.
    const float shadow_dist = texture(_cubemap, tex_coord).r;

    // Distance from light to fragment.
    const float frag_dist = length(io_position - _light.position_) / _light.shadow_distance_;

    return dot(io_position - _light.position_, _frag_normal) < 0.0f && // Only cast shadows on surfaces facing the light.
            shadow_dist + _bias < frag_dist && frag_dist < 1.0f; // Is the shadow closer to the light than the fragment?
}

bool cast_spot_shadow(const in light _light, const in sampler2D _texture, const in vec3 _frag_normal, float _bias) {
    const vec4 light_clip_pos = _light.view_projection_matrix_ * u_inv_view_matrix * vec4(io_position, 1.0f); // Fragment position in the light's clip space.
    const vec3 light_ndc_pos = light_clip_pos.xyz / light_clip_pos.w; // Fragment position in the light's NDC space.
    const vec2 tex_coord = (light_ndc_pos.xy * 0.5f) + vec2(0.5f, 0.5f); // Normalise from the [-1, 1] range to the [0, 1] range.

    // Distance from light to shadow.
    const float shadow_dist = texture(_texture, tex_coord).r;

    // Distance from light to fragment.
    const float frag_dist = 0.5f * light_ndc_pos.z + 0.5f; // Normalise from the [-1, 1] range to the [0, 1] range.

    return 0.0f <= tex_coord.x && tex_coord.x <= 1.0f && // Ensure that the fragment can be seen by the light.
            0.0f <= tex_coord.y && tex_coord.y <= 1.0f &&
            dot(_light.direction_, _frag_normal) < 0.0f && // Only cast shadows on surfaces facing the light.
            shadow_dist + _bias < frag_dist && frag_dist < 1.0f; // Is the shadow closer to the light than the fragment?
}

bool cast_directional_shadow(const in light _light, const in sampler2D _texture, const in vec3 _frag_normal, float _bias) {
    return cast_spot_shadow(_light, _texture, _frag_normal, _bias);
}

void check_cast_shadow(const in vec3 _frag_normal) {
    const float bias = 0.005f;
    for (int i = 0; i < u_num_lights; ++i) {
        g_cast_shadow[i] =
            (u_lights[i].mode_ == light_point && cast_point_shadow(u_lights[i], u_cubemap_shadows[i], _frag_normal, bias)) ||
            (u_lights[i].mode_ == light_spot && cast_spot_shadow(u_lights[i], u_texture_shadows[i], _frag_normal, bias)) ||
            (u_lights[i].mode_ == light_directional && cast_directional_shadow(u_lights[i], u_texture_shadows[i], _frag_normal, bias));
    }
}

void main() {
    const vec2 tex_coord = get_tex_coord();
    const vec3 normal = get_normal(tex_coord);
    const float gloss = get_gloss(tex_coord);

    check_cast_shadow(normal);

    const vec4 diffuse = get_diffuse(tex_coord) * light_diffuse(io_position, normal);
    const vec4 specular = get_specular(tex_coord) * light_specular(io_position, normal, gloss);
    const vec4 ambient = get_diffuse(tex_coord) * u_ambient_light;
    const vec3 phong = (ambient + diffuse + specular).rgb;

    out_colour = vec4(phong, 1.0f);
    out_position = vec4(io_position, 1.0f);
    out_normal = vec4(io_normal, 1.0f);
}