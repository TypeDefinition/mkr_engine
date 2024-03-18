#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    // Forward buffer
    class forward_buffer : public framebuffer {
    public:
        enum colour_attachments : int32_t {
            composite = 0, // The scene rendered with geometry and lights.
            position,
            normal,
            num_attachments,
        };

        forward_buffer(uint32_t _width, uint32_t _height);

        virtual ~forward_buffer();
    };
} // mkr