#include "graphics/framebuffer/geometry_buffer.h"

namespace mkr {
    geometry_buffer::geometry_buffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        // We use GL_RGBA16F over GL_RGB16F as GPUs generally prefer 4-component formats to 3-component formats due to byte alignment.
        // Some drivers may fail to complete the framebuffer otherwise. [https://www.khronos.org/opengl/wiki/Image_Format#Texture_and_Renderbuffer]
        colour_attachments_.resize(colour_attachments::num_attachments);
        colour_attachments_[colour_attachments::position] = std::make_unique<texture_2d>("position", _width, _height, sized_format::rgba16f);
        colour_attachments_[colour_attachments::normal] = std::make_unique<texture_2d>("normal", _width, _height, sized_format::rgba16f);
        colour_attachments_[colour_attachments::diffuse] = std::make_unique<texture_2d>("diffuse", _width, _height, sized_format::rgba8);
        colour_attachments_[colour_attachments::specular] = std::make_unique<texture_2d>("specular", _width, _height, sized_format::rgba8);
        colour_attachments_[colour_attachments::gloss] = std::make_unique<texture_2d>("gloss", _width, _height, sized_format::r16f);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Depth-Stencil attachments.
        depth_stencil_attachment_ = std::make_unique<texture_2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
        glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete geometry buffer");
        }
    }

    geometry_buffer::~geometry_buffer() {
        glDeleteFramebuffers(1, &handle_);
    }
} // mkr