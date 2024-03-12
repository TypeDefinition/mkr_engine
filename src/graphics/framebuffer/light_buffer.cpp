#include "graphics/framebuffer/light_buffer.h"

namespace mkr {
    light_buffer::light_buffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        colour_attachments_.resize(num_lbuffer_attachments);
        colour_attachments_[lbuffer_composite] = std::make_unique<texture_2d>("composite", _width, _height, sized_format::rgba8);
        colour_attachments_[lbuffer_diffuse] = std::make_unique<texture_2d>("diffuse", _width, _height, sized_format::rgba8);
        colour_attachments_[lbuffer_specular] = std::make_unique<texture_2d>("specular", _width, _height, sized_format::rgba8);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Back buffers.
        diffuse_back_ = std::make_unique<texture_2d>("diffuse", _width, _height, sized_format::rgba8);
        specular_back_ = std::make_unique<texture_2d>("specular", _width, _height, sized_format::rgba8);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete light buffer");
        }
    }

    light_buffer::~light_buffer() {
        glDeleteFramebuffers(1, &handle_);
    }

    void light_buffer::swap_buffers() {
        colour_attachments_[lbuffer_diffuse].swap(diffuse_back_);
        colour_attachments_[lbuffer_specular].swap(specular_back_);
        glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + lbuffer_diffuse, colour_attachments_[lbuffer_diffuse]->handle(), 0);
        glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + lbuffer_specular, colour_attachments_[lbuffer_specular]->handle(), 0);
    }
} // mkr