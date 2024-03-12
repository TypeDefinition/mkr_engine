#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    // Forward buffer
    class forward_buffer : public framebuffer {
    public:
        forward_buffer(uint32_t _width, uint32_t _height);

        virtual ~forward_buffer();
    };
} // mkr