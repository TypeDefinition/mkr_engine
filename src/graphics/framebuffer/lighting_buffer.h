#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class lighting_buffer : public framebuffer {
    protected:
        /* In order to support unlimited lights, it may be necessary to run the lighting pass multiple times.
           For each run, we calculate the lighting of each light of this run. Then we read the buffers of the previous runs to get their values.
           Then we add the results together, and write to the diffuse and specular buffers again.
           In order to do that, we will need 2 diffuse and specular buffers to prevent read and writing to the same buffer in the same run.
           Otherwise, this will result in a read-write race condition and produce artifacts as multiple fragments are reading and writing to the same buffers. */
        std::unique_ptr<texture> diffuse_back_;
        std::unique_ptr<texture> specular_back_;

    public:
        enum colour_attachments : int32_t {
            colour = 0, // The scene rendered with geometry and lights.
            diffuse, // Light diffuse colour to be used as an input by the next pass in order to support unlimited lights.
            specular, // Light specular colour to be used as an input by the next pass in order to support unlimited lights.
            num_attachments,
        };

        lighting_buffer(uint32_t _width, uint32_t _height) : framebuffer(_width, _height) {
            // Create GL buffer.
            glCreateFramebuffers(1, &handle_);

            // Colour attachments.
            colour_attachments_.resize(colour_attachments::num_attachments);
            colour_attachments_[colour_attachments::colour] = std::make_unique<texture2d>("colour", _width, _height, sized_format::rgba8);
            colour_attachments_[colour_attachments::diffuse] = std::make_unique<texture2d>("diffuse", _width, _height, sized_format::rgba8);
            colour_attachments_[colour_attachments::specular] = std::make_unique<texture2d>("specular", _width, _height, sized_format::rgba8);
            for (auto i = 0; i < colour_attachments_.size(); ++i) {
                glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
            }

            // Back buffers.
            diffuse_back_ = std::make_unique<texture2d>("diffuse", _width, _height, sized_format::rgba8);
            specular_back_ = std::make_unique<texture2d>("specular", _width, _height, sized_format::rgba8);

            // Completeness check.
            if (!is_complete()) {
                throw std::runtime_error("incomplete lighting buffer");
            }
        }

        virtual ~lighting_buffer() {
            glDeleteFramebuffers(1, &handle_);
        }

        virtual void swap_buffers() {
            colour_attachments_[colour_attachments::diffuse].swap(diffuse_back_);
            colour_attachments_[colour_attachments::specular].swap(specular_back_);
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + colour_attachments::diffuse, colour_attachments_[colour_attachments::diffuse]->handle(), 0);
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + colour_attachments::specular, colour_attachments_[colour_attachments::specular]->handle(), 0);
        }
    };
} // mkr