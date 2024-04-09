#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class geometry_buffer : public framebuffer {
    public:
        enum colour_attachments : int32_t {
            position = 0,
            normal,
            diffuse,
            specular,
            gloss,
            num_attachments,
        };

        geometry_buffer(uint32_t _width, uint32_t _height) : framebuffer(_width, _height) {
            // Create GL buffer.
            glCreateFramebuffers(1, &handle_);

            // Colour attachments.
            /* Be careful when using GL_RGB16F over GL_RGBA16F as GPUs generally prefer 4-component formats to 3-component formats due to byte alignment.
               Some drivers may fail to complete the framebuffer. [https://www.khronos.org/opengl/wiki/Image_Format#Texture_and_Renderbuffer]
               It should be fine since we're using textures, but some of these formats may not be supported by renderbuffers. */
            colour_attachments_.resize(colour_attachments::num_attachments);
            colour_attachments_[colour_attachments::position] = std::make_unique<texture2d>("position", _width, _height, sized_format::rgb16f);
            colour_attachments_[colour_attachments::normal] = std::make_unique<texture2d>("normal", _width, _height, sized_format::rgb16f);
            colour_attachments_[colour_attachments::diffuse] = std::make_unique<texture2d>("diffuse", _width, _height, sized_format::rgba8);
            colour_attachments_[colour_attachments::specular] = std::make_unique<texture2d>("specular", _width, _height, sized_format::rgba8);
            colour_attachments_[colour_attachments::gloss] = std::make_unique<texture2d>("gloss", _width, _height, sized_format::r16f);
            for (auto i = 0; i < colour_attachments_.size(); ++i) {
                glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
            }

            // Depth-Stencil attachments.
            depth_stencil_attachment_ = std::make_unique<texture2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
            glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

            // Completeness check.
            if (!is_complete()) {
                throw std::runtime_error("incomplete geometry buffer");
            }
        }

        virtual ~geometry_buffer() {
            glDeleteFramebuffers(1, &handle_);
        }
    };
} // mkr