#pragma once

#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <maths/colour.h>
#include "graphics/texture.h"

namespace mkr {
    enum colour_attachments {
        gbuffer_position = 0,
        gbuffer_normal,
        gbuffer_albedo,
        gbuffer_specular,
        gbuffer_gloss,
        num_gbuffer_attachments,

        lbuffer_composite = 0, // The scene rendered with geometry and lights.
        lbuffer_diffuse, // Light diffuse colour to be used as an input by the next pass in order to support unlimited lights.
        lbuffer_specular, // Light specular colour to be used as an input by the next pass in order to support unlimited lights.
        num_lbuffer_attachments,

        fbuffer_composite = 0, // The scene rendered with geometry and lights.
        fbuffer_position,
        fbuffer_normal,
        num_fbuffer_attachments,

        pbuffer_composite = 0,  // The scene rendered with geometry and lights.
        num_pbuffer_attachments,
    };

    class framebuffer {
    protected:
        std::vector<std::shared_ptr<texture_2d>> colour_attachments_;
        std::shared_ptr<texture_2d> depth_stencil_attachment_;
        GLuint handle_;

        framebuffer();

    public:
        virtual ~framebuffer() = default;

        bool is_complete() const;

        std::shared_ptr<texture_2d> get_colour_attachment(colour_attachments _attachment);

        std::shared_ptr<const texture_2d> get_colour_attachment(colour_attachments _attachment) const;

        std::shared_ptr<texture_2d> get_depth_stencil_attachment();

        std::shared_ptr<const texture_2d> get_depth_stencil_attachment() const;

        void bind();

        void blit_to(std::shared_ptr<framebuffer> _other, bool _colour, bool _depth, bool _stencil,
                     int32_t _src_x0, int32_t _src_y0, int32_t _src_x1, int32_t _src_y1,
                     int32_t _dst_x0, int32_t _dst_y0, int32_t _dst_x1, int32_t _dst_y1);

        virtual void set_read_colour_attachment(colour_attachments _attachment);

        virtual void set_draw_colour_attachment(colour_attachments _attachment);

        virtual void set_draw_colour_attachment_all();

        virtual void clear_colour(colour_attachments _attachment, const colour& _colour = colour::black);

        virtual void clear_colour_all(const colour& _colour = colour::black);

        virtual void clear_depth_stencil(float _depth = 1.0f, int32_t _stencil = 0);
    };

    // Default Buffer
    class dbuffer : public framebuffer {
    public:
        dbuffer() = default;

        virtual ~dbuffer() = default;

        virtual void set_draw_colour_attachment_all();

        virtual void clear_colour_all(const colour& _colour = colour::black);
    };

    // Geometry Buffer
    class gbuffer : public framebuffer {
    public:
        gbuffer(uint32_t _width, uint32_t _height);

        virtual ~gbuffer();
    };

    // Light Buffer
    class lbuffer : public framebuffer {
    protected:
        // In order to support unlimited lights, it may be necessary to run the lighting pass multiple times.
        // For each run, we calculate the lighting of each light of this run. Then we read the buffers of the previous runs to get their values.
        // Then we add the results together, and write to the diffuse and specular buffers again.
        // In order to do that, we will need 2 diffuse and specular buffers to prevent read and writing to the same buffer in the same run.
        // Otherwise, this will result in a read-write race condition and produce artifacts as multiple fragments are reading and writing to the same buffers.
        std::shared_ptr<texture_2d> diffuse_back_;
        std::shared_ptr<texture_2d> specular_back_;

    public:
        lbuffer(uint32_t _width, uint32_t _height);

        virtual ~lbuffer();

        virtual void swap_buffers();
    };

    // Forward buffer
    class fbuffer : public framebuffer {
    public:
        fbuffer(uint32_t _width, uint32_t _height);

        virtual ~fbuffer();
    };

    // Post-Process Buffer
    class pbuffer : public framebuffer {
    protected:
        // Similar to lbuffer.
        std::shared_ptr<texture_2d> composite_back_;

    public:
        pbuffer(uint32_t _width, uint32_t _height);

        virtual ~pbuffer();

        virtual void swap_buffers();
    };
}