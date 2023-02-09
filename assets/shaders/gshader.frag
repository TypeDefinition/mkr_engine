#version 460 core

/* IMPORTANT: All transforms in the fragment shader is in camera space, where the camera is at the origin. */

layout (location = 0) out vec4 out_position;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_albedo;
layout (location = 3) out vec4 out_specular;
layout (location = 4) out float out_gloss;

// Inputs
in io_block {
    vec3 io_tex_coord;
    vec3 io_vertex_position; // Vertex position in camera space.
    vec3 io_vertex_normal; // Vertex normal in camera space.
    mat3 io_normal_matrix;
    mat3 io_tbn_matrix; // Converts from tangent space to camera space.
    mat3 io_tbn_inv_matrix; // Converts from camera space to tangent space.
};

// Uniforms
uniform vec4 u_albedo_colour;
uniform float u_gloss;
uniform float u_displacement_scale;

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

vec2 parallax_mapping(const vec2 _tex_coord) {
    // Get the direction of the camera to the fragment's surface in tangent space.
    vec3 view_direction = normalize(io_tbn_inv_matrix * io_vertex_position);
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
    return (view_direction.xy / -view_direction.z) * depth * u_displacement_scale; // Parallax Mapping without Offset Limiting
}

vec2 parallax_occlusion_mapping(const vec2 _tex_coord) {
    const vec2 max_uv_displacement = parallax_mapping(_tex_coord);
    const int min_displacement_samples = 8;
    const int max_displacement_samples = 32;
    const float dot_product = dot(io_vertex_normal, normalize(-io_vertex_position));
    const int num_samples = int(mix(max_displacement_samples, min_displacement_samples, dot_product));

    float previous_layer_depth = 0.0f;
    vec2 previous_uv_displacement = vec2(0.0f, 0.0f);
    float previous_texture_depth = 0.0f;
    for (int i = 0; i < num_samples; ++i) {
        const float current_layer_depth = float(i)/float(num_samples);
        const vec2 current_uv_displacement = current_layer_depth * max_uv_displacement;
        const float current_texture_depth = 1.0f - texture(u_texture_displacement, _tex_coord + current_uv_displacement).r;

        if (current_layer_depth > current_texture_depth) {
            const float excess_depth = current_layer_depth - current_texture_depth;
            const float lacking_depth = previous_texture_depth - previous_layer_depth;
            return mix(previous_uv_displacement, current_uv_displacement, lacking_depth / (lacking_depth + excess_depth));
        }

        previous_layer_depth = current_layer_depth;
        previous_uv_displacement = current_uv_displacement;
        previous_texture_depth = current_texture_depth;
    }

    return max_uv_displacement;
}

vec2 get_tex_coord() {
    return u_texture_displacement_enabled ? (io_tex_coord.xy + parallax_occlusion_mapping(io_tex_coord.xy)) : io_tex_coord.xy;
}

vec3 get_normal(const in vec2 _tex_coord) {
    if (u_texture_normal_enabled) {
        vec3 normal = texture(u_texture_normal, _tex_coord).rgb;

        normal *= 2.0f;
        normal -= vec3(1.0f, 1.0f, 1.0f);
        normal = normalize(normal);

        return io_tbn_matrix * normal;
    }
    return io_vertex_normal;
}

vec4 get_albedo(const in vec2 _tex_coord) {
    return u_texture_albedo_enabled ? texture(u_texture_albedo, _tex_coord) * u_albedo_colour : u_albedo_colour;
}

vec4 get_specular(const in vec2 _tex_coord) {
    return u_texture_specular_enabled ? texture(u_texture_specular, _tex_coord) : vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

float get_gloss(const in vec2 _tex_coord) {
    return u_texture_gloss_enabled ? texture(u_texture_gloss, _tex_coord).r * u_gloss : u_gloss;
}

void main() {
    const vec2 tex_coord = get_tex_coord();
    const vec3 normal = get_normal(tex_coord);
    const vec4 albedo = get_albedo(tex_coord);
    const vec4 specular = get_specular(tex_coord);
    const float gloss = get_gloss(tex_coord);

    out_position = vec4(io_vertex_position, 1.0f);
    out_normal = vec4(normal, 1.0f);
    out_albedo = albedo;
    out_specular = specular;
    out_gloss = gloss;
}