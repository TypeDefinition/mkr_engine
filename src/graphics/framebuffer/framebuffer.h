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
        std::vector<std::unique_ptr<texture>> colour_attachments_;
        std::unique_ptr<texture> depth_stencil_attachment_;
        const uint32_t width_, height_;

        framebuffer(uint32_t _width, uint32_t _height) : width_(_width), height_(_height) {}

    public:
        virtual ~framebuffer() = default;

        bool is_complete() const;

        inline uint32_t width() const { return width_; }

        inline uint32_t height() const { return height_; }

        texture* get_colour_attachment(int32_t _attachment);

        const texture* get_colour_attachment(int32_t _attachment) const;

        texture* get_depth_stencil_attachment();

        const texture* get_depth_stencil_attachment() const;

        void bind();

        void blit_to(framebuffer* _other, bool _colour, bool _depth, bool _stencil,
                     int32_t _src_x0, int32_t _src_y0, int32_t _src_x1, int32_t _src_y1,
                     int32_t _dst_x0, int32_t _dst_y0, int32_t _dst_x1, int32_t _dst_y1);

        virtual void set_read_colour_attachment(int32_t _attachment);

        virtual void set_draw_colour_attachment(int32_t _attachment);

        virtual void set_draw_colour_attachment_all();

        virtual void clear_colour(int32_t _attachment, const colour& _colour = colour::black());

        virtual void clear_colour_all(const colour& _colour = colour::black());

        virtual void clear_depth_stencil(float _depth = 1.0f, int32_t _stencil = 0);

        static void bind_default_buffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

        static void clear_default_buffer_colour(const colour& _colour = colour::black()) { glClearNamedFramebufferfv(0, GL_COLOR, 0, (GLfloat*)&_colour.r_); }

        static void clear_default_depth_stencil(float _depth = 1.0f, int32_t _stencil = 0) { glClearNamedFramebufferfi(0, GL_DEPTH_STENCIL, 0, _depth, _stencil); }
    };
}