#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class alpha_buffer : public framebuffer {
    public:
        enum colour_attachments : int32_t {
            accumulation = 0,
            revealage,

            num_attachments,
        };

        alpha_buffer(uint32_t _width, uint32_t _height) : framebuffer(_width, _height) {
            // Create GL buffer.
            glCreateFramebuffers(1, &handle_);

            // Colour attachments.
            colour_attachments_.resize(colour_attachments::num_attachments);
            colour_attachments_[colour_attachments::accumulation] = std::make_unique<texture2d>("accum", _width, _height, sized_format::rgba16f);
            colour_attachments_[colour_attachments::revealage] = std::make_unique<texture2d>("reveal", _width, _height, sized_format::r8);
            for (auto i = 0; i < colour_attachments_.size(); ++i) {
                glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
            }

            // Depth-Stencil attachments.
            depth_stencil_attachment_ = std::make_unique<texture2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
            glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

            // Completeness check.
            if (!is_complete()) {
                throw std::runtime_error("incomplete transparent buffer");
            }
        }

        virtual ~alpha_buffer() {
            glDeleteFramebuffers(1, &handle_);
        }
    };
} // mkr