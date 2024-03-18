#include "graphics/framebuffer/post_proc_buffer.h"

namespace mkr {
    post_proc_buffer::post_proc_buffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        colour_attachments_.resize(colour_attachments::num_attachments);
        colour_attachments_[colour_attachments::composite] = std::make_unique<texture_2d>("composite", _width, _height, sized_format::rgba8);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        composite_back_ = std::make_unique<texture_2d>("composite", _width, _height, sized_format::rgba8);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete post process buffer");
        }
    }

    post_proc_buffer::~post_proc_buffer() {
        glDeleteFramebuffers(1, &handle_);
    }

    void post_proc_buffer::swap_buffers() {
        colour_attachments_[colour_attachments::composite].swap(composite_back_);
        glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + colour_attachments::composite, colour_attachments_[colour_attachments::composite]->handle(), 0);
    }
} // mkr