#pragma once

#include <cstdint>
#include <maths/colour.h>

namespace mkr {
    class lighting {
    public:
        lighting() = delete;

        static constexpr uint32_t max_lights = 4; // Must match shader!

        static colour ambient_light_;
    };
}