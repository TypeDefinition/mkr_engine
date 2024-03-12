#include "graphics/framebuffer/shadow_buffer.h"

namespace mkr {
    shadow_buffer::shadow_buffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        glNamedFramebufferDrawBuffer(handle_, GL_NONE);

        // Depth-Stencil attachments.
        depth_stencil_attachment_ = std::make_unique<texture_2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
        glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

        if (!is_complete()) {
            throw std::runtime_error("incomplete shadow buffer");
        }
    }

    shadow_buffer::~shadow_buffer() {
        glDeleteFramebuffers(1, &handle_);
    }
} // mkr