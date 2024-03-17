#pragma once

#include <cstdint>

namespace mkr {
    enum texture_unit : int32_t {
        texture_skybox = 0, // Skybox

        texture_diffuse, // G-Pass, F-Pass
        texture_normal, // G-Pass, F-Pass
        texture_specular, // G-Pass, F-Pass
        texture_gloss, // G-Pass, F-Pass
        texture_displacement, // G-Pass, F-Pass

        texture_frag_position, // L-Pass, P-Pass
        texture_frag_normal, // L-Pass, P-Pass
        texture_frag_diffuse, // L-Pass
        texture_frag_specular, // L-Pass
        texture_frag_gloss, // L-Pass
        texture_light_diffuse, // L-Pass
        texture_light_specular, // L-Pass

        texture_composite, // P-Pass
        texture_depth_stencil, // P-Pass
    };
} // mkr