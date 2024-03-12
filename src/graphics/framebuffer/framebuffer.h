#pragma once

#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "maths/colour.h"
#include "graphics/texture/texture.h"

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
        std::vector<std::unique_ptr<texture_2d>> colour_attachments_;
        std::unique_ptr<texture_2d> depth_stencil_attachment_;
        GLuint handle_;

        framebuffer();

    public:
        virtual ~framebuffer() = default;

        bool is_complete() const;

        texture_2d* get_colour_attachment(colour_attachments _attachment);

        const texture_2d* get_colour_attachment(colour_attachments _attachment) const;

        texture_2d* get_depth_stencil_attachment();

        const texture_2d* get_depth_stencil_attachment() const;

        void bind();

        void blit_to(framebuffer* _other, bool _colour, bool _depth, bool _stencil,
                     int32_t _src_x0, int32_t _src_y0, int32_t _src_x1, int32_t _src_y1,
                     int32_t _dst_x0, int32_t _dst_y0, int32_t _dst_x1, int32_t _dst_y1);

        virtual void set_read_colour_attachment(colour_attachments _attachment);

        virtual void set_draw_colour_attachment(colour_attachments _attachment);

        virtual void set_draw_colour_attachment_all();

        virtual void clear_colour(colour_attachments _attachment, const colour& _colour = colour::black);

        virtual void clear_colour_all(const colour& _colour = colour::black);

        virtual void clear_depth_stencil(float _depth = 1.0f, int32_t _stencil = 0);
    };
}