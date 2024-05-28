#include <light.frag>

// Uniforms
uniform sampler2D u_texture_shadows[max_lights];
uniform samplerCube u_cubemap_shadows[max_lights];

bool cast_point_shadow(const in samplerCube _cubemap,
                       const in vec3 _pos, const in vec3 _normal,
                       const in mat4 _inv_view_matrix,
                       const in vec3 _light_pos,
                       float _cast_dist) {
    const float bias = 0.001f;

    const vec4 frag_world_pos = _inv_view_matrix * vec4(_pos, 1.0f); // World space.
    const vec4 light_world_pos = _inv_view_matrix * vec4(_light_pos, 1.0f); // World space.
    const vec3 tex_coord = (frag_world_pos - light_world_pos).xyz; // No need to normalise the tex_coord for a cubemap.

    // Distance from light to shadow.
    const float shadow_dist = texture(_cubemap, tex_coord).r;

    // Distance from light to fragment.
    const float frag_dist = length(_pos - _light_pos) / _cast_dist; // Normalise to the [0, 1] range.

    return shadow_dist + bias < frag_dist && frag_dist < 1.0f && // Is the shadow closer to the light than the fragment?
           dot(_pos - _light_pos, _normal) < 0.0f; // Only cast shadows on surfaces facing the light.

}

bool cast_spot_shadow(const in sampler2D _texture,
                      const in vec3 _pos, const in vec3 _normal,
                      const in mat4 _inv_view_matrix,
                      const in mat4 _light_vp_mat, const in vec3 _light_dir) {
    const float bias = 0.001f;

    const vec4 light_clip_pos = _light_vp_mat * _inv_view_matrix * vec4(_pos, 1.0f); // Fragment position in the light's clip space.
    const vec3 light_ndc_pos = light_clip_pos.xyz / light_clip_pos.w; // Fragment position in the light's NDC space.
    const vec2 tex_coord = (light_ndc_pos.xy * 0.5f) + vec2(0.5f, 0.5f); // Normalise from the [-1, 1] range to the [0, 1] range.

    // Distance from light to shadow.
    const float shadow_dist = texture(_texture, tex_coord).r;

    // Distance from light to fragment.
    const float frag_dist = 0.5f * light_ndc_pos.z + 0.5f; // Normalise from the [-1, 1] range to the [0, 1] range.

    return shadow_dist + bias < frag_dist && frag_dist < 1.0f && // Is the shadow closer to the light than the fragment?
           dot(_light_dir, _normal) < 0.0f && // Only cast shadows on surfaces facing the light.
           0.0f <= tex_coord.x && tex_coord.x <= 1.0f && // Ensure that the fragment can be seen by the light.
           0.0f <= tex_coord.y && tex_coord.y <= 1.0f;
}

bool cast_directional_shadow(const in sampler2D _texture,
                             const in vec3 _pos, const in vec3 _normal,
                             const in mat4 _inv_view_matrix,
                             const in mat4 _light_vp_mat, const in vec3 _light_dir) {
    return cast_spot_shadow(_texture, _pos, _normal, _inv_view_matrix, _light_vp_mat, _light_dir);
}

void get_cast_shadow(const in vec3 _position, const in vec3 _normal,
                     const in mat4 _inv_view_matrix,
                     const in light _lights[max_lights],
                     int _num_lights,
                     inout bool cast_shadow[max_lights]) {
    for (int i = 0; i < _num_lights; ++i) {
        cast_shadow[i] = (_lights[i].mode_ == light_point && cast_point_shadow(u_cubemap_shadows[i], _position, _normal, _inv_view_matrix, _lights[i].position_, _lights[i].shadow_distance_)) ||
        (_lights[i].mode_ == light_spot && cast_spot_shadow(u_texture_shadows[i], _position, _normal, _inv_view_matrix, _lights[i].view_projection_matrix_, _lights[i].direction_)) ||
        (_lights[i].mode_ == light_directional && cast_directional_shadow(u_texture_shadows[i], _position, _normal, _inv_view_matrix, _lights[i].view_projection_matrix_, _lights[i].direction_));
    }
}