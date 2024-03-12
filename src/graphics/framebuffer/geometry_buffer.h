#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    // Geometry Buffer
    class geometry_buffer : public framebuffer {
    public:
        geometry_buffer(uint32_t _width, uint32_t _height);

        virtual ~geometry_buffer();
    };
} // mkr