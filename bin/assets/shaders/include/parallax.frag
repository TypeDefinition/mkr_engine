// Parallax Occlusion
vec2 parallax(const in sampler2D _texture, const in vec2 _tex_coord,
                        float _disp_scale,
                        const in vec3 _frag_pos, const in mat3 _tbn_matrix,
                        const in vec3 _normal) {
    const int min_layers = 4;
    const int max_layers = 16;
    const int num_layers = int(mix(max_layers, min_layers, dot(_normal, normalize(-_frag_pos))));

    const vec3 view_dir = normalize(transpose(_tbn_matrix) * _frag_pos); // Convert from camera space to tangent space.
    const vec2 uv_delta = -view_dir.xy / view_dir.z / num_layers * _disp_scale; // How much to shift the uv coordinates per layer.

    vec2 prev_uv_offset = vec2(0.0f, 0.0f);
    float prev_layer_depth = 0.0f;
    float prev_tex_depth = 0.0f;
    for (int i = 0; i < num_layers; ++i) {
        const vec2 curr_uv_offset = uv_delta * i;
        const float curr_layer_depth = float(i) / float(num_layers);
        const float curr_tex_depth = 1.0f - texture(_texture, _tex_coord + curr_uv_offset).r;

        if (curr_tex_depth < curr_layer_depth) {
            const float excess_depth = curr_layer_depth - curr_tex_depth;
            const float lacking_depth = prev_tex_depth - prev_layer_depth;
            return _tex_coord + mix(prev_uv_offset, curr_uv_offset, lacking_depth / (lacking_depth + excess_depth));
        }

        prev_layer_depth = curr_layer_depth;
        prev_uv_offset = curr_uv_offset;
        prev_tex_depth = curr_tex_depth;
    }

    return _tex_coord;
}