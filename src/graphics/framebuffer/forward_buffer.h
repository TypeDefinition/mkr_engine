#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class forward_buffer : public framebuffer {
    public:
        enum colour_attachments : int32_t {
            colour = 0, // The scene rendered with geometry and lights.
            position,
            normal,

            num_attachments,
        };

        forward_buffer(uint32_t _width, uint32_t _height) : framebuffer(_width, _height) {
            // Create GL buffer.
            glCreateFramebuffers(1, &handle_);

            // Colour attachments.
            colour_attachments_.resize(colour_attachments::num_attachments);
            colour_attachments_[colour_attachments::colour] = std::make_unique<texture2d>("colour", _width, _height, sized_format::rgba8);
            colour_attachments_[colour_attachments::position] = std::make_unique<texture2d>("position", _width, _height, sized_format::rgba16f);
            colour_attachments_[colour_attachments::normal] = std::make_unique<texture2d>("normal", _width, _height, sized_format::rgba16f);
            for (auto i = 0; i < colour_attachments_.size(); ++i) {
                glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
            }

            // Depth-Stencil attachments.
            depth_stencil_attachment_ = std::make_unique<texture2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
            glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

            // Completeness check.
            if (!is_complete()) {
                throw std::runtime_error("incomplete forward buffer");
            }
        }

        virtual ~forward_buffer() {
            glDeleteFramebuffers(1, &handle_);
        }
    };
} // mkr