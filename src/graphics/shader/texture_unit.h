#pragma once

#include <cstdint>
#include "graphics/lighting/lighting.h"

namespace mkr {
    // Maximum 32 texture units in OpenGL.
    enum texture_unit : int32_t {
        texture_skybox = 0,

        texture_debug,

        texture_diffuse,
        texture_normal,
        texture_specular,
        texture_gloss,
        texture_displacement,

        texture_shadows0,
        cubemap_shadows0 = lighting::max_lights + texture_shadows0,

        // texture_frag_position, // L-Pass, P-Pass
        // texture_frag_normal, // L-Pass, P-Pass
        // texture_frag_diffuse, // L-Pass
        // texture_frag_specular, // L-Pass
        // texture_frag_gloss, // L-Pass
        // texture_light_diffuse, // L-Pass
        // texture_light_specular, // L-Pass

        // texture_composite, // P-Pass
        // texture_depth_stencil, // P-Pass

        num_texture_units = lighting::max_lights + cubemap_shadows0,
    };
} // mkr