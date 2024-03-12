#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    // Shadow Buffer
    class shadow_buffer : public framebuffer {
    public:
        shadow_buffer(uint32_t _width, uint32_t _height);

        virtual ~shadow_buffer();
    };
} // mkr