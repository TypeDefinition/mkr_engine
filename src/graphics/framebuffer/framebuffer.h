#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "maths/colour.h"
#include "graphics/texture/texture.h"

namespace mkr {
    class framebuffer {
    protected:
        GLuint handle_ = 0;
        std::vector<std::unique_ptr<texture_2d>> colour_attachments_;
        std::unique_ptr<texture_2d> depth_stencil_attachment_;

        framebuffer() = default;

    public:
        virtual ~framebuffer() = default;

        bool is_complete() const;

        texture_2d* get_colour_attachment(int32_t _attachment);

        const texture_2d* get_colour_attachment(int32_t _attachment) const;

        texture_2d* get_depth_stencil_attachment();

        const texture_2d* get_depth_stencil_attachment() const;

        void bind();

        void blit_to(framebuffer* _other, bool _colour, bool _depth, bool _stencil,
                     int32_t _src_x0, int32_t _src_y0, int32_t _src_x1, int32_t _src_y1,
                     int32_t _dst_x0, int32_t _dst_y0, int32_t _dst_x1, int32_t _dst_y1);

        virtual void set_read_colour_attachment(int32_t _attachment);

        virtual void set_draw_colour_attachment(int32_t _attachment);

        virtual void set_draw_colour_attachment_all();

        virtual void clear_colour(int32_t _attachment, const colour& _colour = colour::black);

        virtual void clear_colour_all(const colour& _colour = colour::black);

        virtual void clear_depth_stencil(float _depth = 1.0f, int32_t _stencil = 0);
    };
}