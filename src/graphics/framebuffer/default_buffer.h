#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class default_buffer : public framebuffer {
    public:
        default_buffer() { handle_ = 0; }

        virtual ~default_buffer() = default;

        virtual void set_draw_colour_attachment_all();

        virtual void clear_colour_all(const colour& _colour = colour::black);
    };
} // mkr