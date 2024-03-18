#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    // Post-Process Buffer
    class post_proc_buffer : public framebuffer {
    public:
        enum colour_attachments : int32_t {
            composite = 0,  // The scene rendered with geometry and lights.
            num_attachments,
        };

    protected:
        // Similar to lbuffer.
        std::unique_ptr<texture_2d> composite_back_;

    public:
        post_proc_buffer(uint32_t _width, uint32_t _height);

        virtual ~post_proc_buffer();

        virtual void swap_buffers();
    };
} // mkr