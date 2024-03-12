#include "graphics/framebuffer/default_buffer.h"

namespace mkr {
    // Default Buffer
    void default_buffer::set_draw_colour_attachment_all() {
        glNamedFramebufferDrawBuffer(handle_, GL_COLOR_ATTACHMENT0);
    }

    void default_buffer::clear_colour_all(const colour& _colour) {
        glClearNamedFramebufferfv(handle_, GL_COLOR, 0, (GLfloat*)&_colour.r_);
    }
}