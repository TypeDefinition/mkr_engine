#pragma once

#include <cstdint>
#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    class light_buffer : public framebuffer {
    public:
        enum colour_attachments : int32_t {
            composite = 0, // The scene rendered with geometry and lights.
            diffuse, // Light diffuse colour to be used as an input by the next pass in order to support unlimited lights.
            specular, // Light specular colour to be used as an input by the next pass in order to support unlimited lights.
            num_attachments,
        };

    protected:
        // In order to support unlimited lights, it may be necessary to run the lighting pass multiple times.
        // For each run, we calculate the lighting of each light of this run. Then we read the buffers of the previous runs to get their values.
        // Then we add the results together, and write to the diffuse and specular buffers again.
        // In order to do that, we will need 2 diffuse and specular buffers to prevent read and writing to the same buffer in the same run.
        // Otherwise, this will result in a read-write race condition and produce artifacts as multiple fragments are reading and writing to the same buffers.
        std::unique_ptr<texture> diffuse_back_;
        std::unique_ptr<texture> specular_back_;

    public:
        light_buffer(uint32_t _width, uint32_t _height);

        virtual ~light_buffer();

        virtual void swap_buffers();
    };
} // mkr