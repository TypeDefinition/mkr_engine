#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class post_buffer : public framebuffer {
    protected:
        std::unique_ptr<texture> colour_back_;

    public:
        enum colour_attachments : int32_t {
            colour = 0,  // The scene rendered with geometry and lights.
            num_attachments,
        };

        post_buffer(uint32_t _width, uint32_t _height) : framebuffer(_width, _height) {
            // Create GL buffer.
            glCreateFramebuffers(1, &handle_);

            // Colour attachments.
            colour_attachments_.resize(colour_attachments::num_attachments);
            colour_attachments_[colour_attachments::colour] = std::make_unique<texture2d>("colour", _width, _height, sized_format::rgba8);
            for (auto i = 0; i < colour_attachments_.size(); ++i) {
                glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
            }

            colour_back_ = std::make_unique<texture2d>("colour", _width, _height, sized_format::rgba8);

            // Completeness check.
            if (!is_complete()) {
                throw std::runtime_error("incomplete post buffer");
            }
        }

        virtual ~post_buffer() {
            glDeleteFramebuffers(1, &handle_);
        }

        virtual void swap_buffers() {
            colour_attachments_[colour_attachments::colour].swap(colour_back_);
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + colour_attachments::colour, colour_attachments_[colour_attachments::colour]->handle(), 0);
        }
    };
} // mkr