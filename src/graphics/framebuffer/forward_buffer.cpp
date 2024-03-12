#include "graphics/framebuffer/forward_buffer.h"

namespace mkr {
    forward_buffer::forward_buffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        colour_attachments_.resize(num_fbuffer_attachments);
        colour_attachments_[fbuffer_composite] = std::make_unique<texture_2d>("composite", _width, _height, sized_format::rgba8);
        colour_attachments_[fbuffer_position] = std::make_unique<texture_2d>("position", _width, _height, sized_format::rgba16f);
        colour_attachments_[fbuffer_normal] = std::make_unique<texture_2d>("normal", _width, _height, sized_format::rgba16f);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Depth-Stencil attachments.
        depth_stencil_attachment_ = std::make_unique<texture_2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
        glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete forward buffer");
        }
    }

    forward_buffer::~forward_buffer() {
        glDeleteFramebuffers(1, &handle_);
    }
} // mkr