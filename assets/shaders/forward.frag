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

vec2 parallax_mapping() {
    /* Get the direction of the camera to the fragment's surface.
    Right now we are in camera space. We need to convert into tangent space.
    Since io_tbn_matrix converts from tangent space to camera space, the transpose (which is also the inverse) converts from camera space to tangent space. */
    vec3 cam_to_frag = normalize(transpose(io_tbn_matrix) * io_vertex_position); // Convert to tangent space.

    /* We are reading a height map, but we want to do our calculations based on depth instead since the results of a depth map looks better than a heightmap.
    By converting the height map to a depth map, it means that a height of 0.9 is a depth of 0.1, and a height of 0.4 is a depth of 0.6. */
    float depth = 1.0f - texture(u_texture_displacement, io_tex_coord.xy).r;

    /* What is interesting to note here is the division of cam_to_frag.xy by cam_to_frag.z.
    As the cam_to_frag vector is normalized cam_to_frag.z will be somewhere in the range between 0.0 and 1.0.
    When cam_to_frag is largely parallel to the surface its z component is close to 0.0 and the division returns a much larger offset.
    compared to when cam_to_frag is largely perpendicular to the surface.
    So basically we're increasing the size of the offset in such a way that it offsets the texture coordinates at a larger scale when looking at a surface from
    an angle compared to when looking at it from the top which gives more realistic results at angles.
    Some people prefer to leave the division by cam_to_frag.z out of the equation as normal Parallax Mapping could produce undesirable results at certain angles.
    The technique is then called Parallax Mapping With Offset Limiting. Choosing which technique to pick is usually a matter of personal preference.
    Do take note that if we do divide by cam_to_frag.z, make sure to negate it. Since we are in tangent space, cam_to_frag.z is a negative number,
    and any division by it will negate our result. So we need to negate cam_to_frag.z to turn it into a positive number first */
    const float displacement_scale = 0.03f;
    return (cam_to_frag.xy / -cam_to_frag.z) * depth * displacement_scale; // Parallax Mapping Without Offset Limiting
    // return cam_to_frag.xy * depth * displacement_scale; // Parallax Mapping With Offset Limiting
}

vec2 parallax_occlusion_mapping() {
    /* Parallax Occlusion Mapping is based on the same principles as Steep Parallax Mapping,
    but instead of taking the texture coordinates of the first depth layer after a collision,
    we're going to linearly interpolate between the depth layer after and before the collision.
    We base the weight of the linear interpolation on how far the surface's height is from the depth layer's value of both layers.
    This is again an approximation, but significantly more accurate than Steep Parallax Mapping. */

    vec2 max_uv_displacement = parallax_mapping();

    /* Rather than take a fixed number of samples each time, we are going to make use of the fact that only from the sides do we need a higher number of samples.
    When view perpendicularly, we do not need as many samples as there is no need to displace anything when a fragment is viewed directly perpendicular.
    When the surface is viewed perpendicularly, the dot product betweem the surface normal and vertexToCam vector is 1, and when viewed sideways, the dor product is 0.
    By doing this, we can select how many samples we actually want to calculate. */
    int min_displacement_samples = 1;
    int max_displacement_samples = 5;
    float dot_product = dot(io_vertex_normal, normalize(-io_vertex_position));
    int num_samples = int(mix(max_displacement_samples, min_displacement_samples, dot_product));

    float current_layer_depth = 0.0f;
    float current_texture_depth = 0.0f;
    vec2 current_uv_displacement = vec2(0.0f, 0.0f);

    float previous_layer_depth = 0.0f;
    float previous_texture_depth = 0.0f;
    vec2 previous_uv_displacement = vec2(0.0f, 0.0f);

    for (int i = 0; i < num_samples; ++i) {
        previous_layer_depth = current_layer_depth;
        previous_texture_depth = current_texture_depth;
        previous_uv_displacement = current_uv_displacement;

        /* We do not have to multiply current_layer_depth & current_texture_depth by u_DisplacementScale as they would cancel each other out.
        Futhermore, doing so will mean we will have to declare another variable so that current_uv_displacement's multiplication is not affected. */
        current_layer_depth = float(i)/float(num_samples);
        current_uv_displacement = current_layer_depth * max_uv_displacement;
        current_texture_depth = 1.0f - texture(u_texture_displacement, io_tex_coord.xy + current_uv_displacement).r;

        // Interpolate between previous_uv_displacement and current_uv_displacement.
        if (current_layer_depth > current_texture_depth) {
            // Currently we have overshot by this much.
            float excess_depth = current_layer_depth - current_texture_depth;
            // Previously we undershot by this much.
            float lacking_depth = previous_texture_depth - previous_layer_depth;

            // return  previous_uv_displacement + (current_uv_displacement - previous_uv_displacement) * (lacking_depth / (lacking_depth + excess_depth));
            return mix(previous_uv_displacement, current_uv_displacement, lacking_depth / (lacking_depth + excess_depth));
        }
    }

    return max_uv_displacement;
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

vec4 get_albedo(const in vec2 _tex_coord) {
    return u_texture_albedo_enabled ? texture(u_texture_albedo, _tex_coord) : u_albedo_colour;
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
        normal = normalize(io_tbn_matrix * normal);

        return normal;
    }
    return io_vertex_normal;
}

vec4 get_specular(const in vec2 _tex_coord) {
    return u_texture_specular_enabled ? texture(u_texture_specular, _tex_coord) : vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

float get_gloss(const in vec2 _tex_coord) {
    return u_texture_gloss_enabled ? texture(u_texture_gloss, _tex_coord).r : u_gloss;
}

vec2 get_tex_coord() {
    if (u_texture_displacement_enabled) {
        return io_tex_coord.xy;
        return io_tex_coord.xy + parallax_mapping();
        return io_tex_coord.xy + parallax_occlusion_mapping();
    }
    return io_tex_coord.xy;
}

void main() {
    const vec2 tex_coord = get_tex_coord();

    if (!u_enable_lights) {
        io_frag_colour = get_albedo(tex_coord);
        return;
    }

    const vec4 albedo = get_albedo(tex_coord);
    const vec3 normal = get_normal(tex_coord);
    const vec4 specular = get_specular(tex_coord);
    const float gloss = get_gloss(tex_coord);

    vec4 ambient_colour = albedo * u_ambient_colour;
    vec4 diffuse_colour = albedo * light_diffuse(io_vertex_position, normal);
    vec4 specular_colour = specular * light_specular(io_vertex_position, normal, gloss);

    io_frag_colour = vec4((ambient_colour + diffuse_colour + specular_colour).rgb, 1.0f);
}