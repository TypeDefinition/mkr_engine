#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class shadow_cubemap_buffer : public framebuffer {
    public:
        shadow_cubemap_buffer(uint32_t _size) : framebuffer(_size, _size) {
            // Create GL buffer.
            glCreateFramebuffers(1, &handle_);

            // No colour attachments.
            glNamedFramebufferReadBuffer(handle_, GL_NONE);
            glNamedFramebufferDrawBuffer(handle_, GL_NONE);

            // Depth-Stencil attachments.
            depth_stencil_attachment_ = std::make_unique<cubemap>("depth_stencil", _size, sized_format::depth24_stencil8);
            glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

            // Completeness check.
            if (!is_complete()) {
                throw std::runtime_error("incomplete shadow cubemap buffer");
            }
        }

        virtual ~shadow_cubemap_buffer() {
            glDeleteFramebuffers(1, &handle_);
        }
    };
} // mkr