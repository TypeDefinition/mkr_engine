#pragma once

#include <cstdint>
#include "graphics/lighting/lighting.h"

namespace mkr {
    // Maximum 32 texture units in OpenGL.
    enum texture_unit : int32_t {
        texture_skybox = 0,

        texture_diffuse = 1,
        texture_specular = 2,
        texture_normal = 3,
        texture_displacement = 4,

        texture_position,
        texture_accumulation,
        texture_revealage,

        // For every shadow index, since only either the texture2d or cubemap will be bound at any given time, they can share the same index.
        texture_shadows0, // Range: [texture_shadows0, texture_shadows0 + lighting::max_lights)
        cubemap_shadows0 = texture_shadows0, // Range: [cubemap_shadows0, cubemap_shadows0 + lighting::max_lights)

        num_texture_units = lighting::max_lights + cubemap_shadows0,
    };
} // mkr