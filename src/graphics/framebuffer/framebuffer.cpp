#include "graphics/framebuffer/framebuffer.h"

namespace mkr {
    // Framebuffer
    framebuffer::framebuffer()
            : handle_(0) {}

    texture_2d* framebuffer::get_colour_attachment(colour_attachments _attachment) {
        return colour_attachments_[_attachment].get();
    }

    const texture_2d* framebuffer::get_colour_attachment(colour_attachments _attachment) const {
        return colour_attachments_[_attachment].get();
    }

    texture_2d* framebuffer::get_depth_stencil_attachment() {
        return depth_stencil_attachment_.get();
    }

    const texture_2d* framebuffer::get_depth_stencil_attachment() const {
        return depth_stencil_attachment_.get();
    }

    bool framebuffer::is_complete() const {
        return GL_FRAMEBUFFER_COMPLETE == glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER);
    }

    void framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, handle_);
    }

    void framebuffer::blit_to(framebuffer* _other, bool _colour, bool _depth, bool _stencil,
                              int32_t _src_x0, int32_t _src_y0, int32_t _src_x1, int32_t _src_y1,
                              int32_t _dst_x0, int32_t _dst_y0, int32_t _dst_x1, int32_t _dst_y1) {
        GLbitfield mask = (_colour ? GL_COLOR_BUFFER_BIT : 0) | (_depth ? GL_DEPTH_BUFFER_BIT : 0) | (_stencil ? GL_STENCIL_BUFFER_BIT : 0);
        // If filter is not GL_NEAREST and mask includes GL_DEPTH_BUFFER_BIT or GL_STENCIL_BUFFER_BIT, no data is transferred and a GL_INVALID_OPERATION error is generated.
        // So for simplicity let's just use GL_NEAREST. Don't act smart and use GL_LINEAR.
        glBlitNamedFramebuffer(handle_, _other->handle_, _src_x0, _src_y0, _src_x1, _src_y1, _dst_x0, _dst_y0, _dst_x1, _dst_y1, mask, GL_NEAREST);
    }

    void framebuffer::set_read_colour_attachment(colour_attachments _attachment) {
        glNamedFramebufferReadBuffer(handle_, GL_COLOR_ATTACHMENT0 + _attachment);
    }

    void framebuffer::set_draw_colour_attachment(colour_attachments _attachment) {
        glNamedFramebufferDrawBuffer(handle_, GL_COLOR_ATTACHMENT0 + _attachment);
    }

    void framebuffer::set_draw_colour_attachment_all() {
        std::vector<GLenum> indices;
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            indices.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        glNamedFramebufferDrawBuffers(handle_, (GLsizei)indices.size(), indices.data());
    }

    void framebuffer::clear_colour(colour_attachments _attachment, const colour& _colour) {
        glClearNamedFramebufferfv(handle_, GL_COLOR, _attachment, (GLfloat*)&_colour.r_);
    }

    void framebuffer::clear_colour_all(const colour& _colour) {
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glClearNamedFramebufferfv(handle_, GL_COLOR, i, (GLfloat*)&_colour.r_);
        }
    }

    void framebuffer::clear_depth_stencil(float _depth, int32_t _stencil) {
        glClearNamedFramebufferfi(handle_, GL_DEPTH_STENCIL, 0, _depth, _stencil);
    }
}