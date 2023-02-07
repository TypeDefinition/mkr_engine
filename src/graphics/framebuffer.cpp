#include "graphics/framebuffer.h"

namespace mkr {
    // Framebuffer
    framebuffer::framebuffer()
            : handle_(0) {}

    std::shared_ptr<texture_2d> framebuffer::get_colour_attachment(colour_attachments _attachment) {
        return colour_attachments_[_attachment];
    }

    std::shared_ptr<const texture_2d> framebuffer::get_colour_attachment(colour_attachments _attachment) const {
        return colour_attachments_[_attachment];
    }

    std::shared_ptr<texture_2d> framebuffer::get_depth_stencil_attachment() {
        return depth_stencil_attachment_;
    }

    std::shared_ptr<const texture_2d> framebuffer::get_depth_stencil_attachment() const {
        return depth_stencil_attachment_;
    }

    bool framebuffer::is_complete() const {
        return GL_FRAMEBUFFER_COMPLETE == glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER);
    }

    void framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, handle_);
    }

    void framebuffer::blit_to(std::shared_ptr<framebuffer> _other, bool _colour, bool _depth, bool _stencil,
                              int32_t _src_x0, int32_t _src_y0, int32_t _src_x1, int32_t _src_y1,
                              int32_t _dst_x0, int32_t _dst_y0, int32_t _dst_x1, int32_t _dst_y1) {
        GLbitfield mask = 0;
        mask |= (_colour ? GL_COLOR_BUFFER_BIT : 0);
        mask |= (_depth ? GL_DEPTH_BUFFER_BIT : 0);
        mask |= (_stencil ? GL_STENCIL_BUFFER_BIT : 0);

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

    // Default Buffer
    void dbuffer::set_draw_colour_attachment_all() {
        glNamedFramebufferDrawBuffer(handle_, GL_COLOR_ATTACHMENT0);
    }

    void dbuffer::clear_colour_all(const colour& _colour) {
        glClearNamedFramebufferfv(handle_, GL_COLOR, 0, (GLfloat*)&_colour.r_);
    }

    // Geometry Buffer
    gbuffer::gbuffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        // We use GL_RGBA16F over GL_RGB16F as GPUs generally prefer 4-component formats to 3-component formats due to byte alignment.
        // Some drivers may fail to complete the framebuffer otherwise. [https://www.khronos.org/opengl/wiki/Image_Format#Texture_and_Renderbuffer]
        colour_attachments_.resize(num_gbuffer_attachments);
        colour_attachments_[gbuffer_position] = std::make_shared<texture_2d>("position", _width, _height, sized_format::rgba16f);
        colour_attachments_[gbuffer_normal] = std::make_shared<texture_2d>("normal", _width, _height, sized_format::rgba16f);
        colour_attachments_[gbuffer_albedo] = std::make_shared<texture_2d>("albedo_colour", _width, _height, sized_format::rgba8);
        colour_attachments_[gbuffer_specular] = std::make_shared<texture_2d>("specular_colour", _width, _height, sized_format::rgba8);
        colour_attachments_[gbuffer_gloss] = std::make_shared<texture_2d>("gloss", _width, _height, sized_format::r16f);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Depth-Stencil attachments.
        depth_stencil_attachment_ = std::make_shared<texture_2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
        glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete geometry buffer");
        }
    }

    gbuffer::~gbuffer() {
        glDeleteFramebuffers(1, &handle_);
    }

    // Light Buffer
    lbuffer::lbuffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        colour_attachments_.resize(num_lbuffer_attachments);
        colour_attachments_[lbuffer_composite] = std::make_shared<texture_2d>("composite", _width, _height, sized_format::rgba8);
        colour_attachments_[lbuffer_diffuse] = std::make_shared<texture_2d>("diffuse", _width, _height, sized_format::rgba8);
        colour_attachments_[lbuffer_specular] = std::make_shared<texture_2d>("specular", _width, _height, sized_format::rgba8);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Back buffers.
        diffuse_back_ = std::make_shared<texture_2d>("diffuse", _width, _height, sized_format::rgba8);
        specular_back_ = std::make_shared<texture_2d>("specular", _width, _height, sized_format::rgba8);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete light buffer");
        }
    }

    lbuffer::~lbuffer() {
        glDeleteFramebuffers(1, &handle_);
    }

    void lbuffer::clear_colour_all(const colour& _colour) {
        framebuffer::clear_colour_all(_colour);

    }

    void lbuffer::swap_buffers() {
        colour_attachments_[lbuffer_diffuse].swap(diffuse_back_);
        colour_attachments_[lbuffer_specular].swap(specular_back_);
        glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + lbuffer_diffuse, colour_attachments_[lbuffer_diffuse]->handle(), 0);
        glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + lbuffer_specular, colour_attachments_[lbuffer_specular]->handle(), 0);
    }

    // Forward Buffer
    fbuffer::fbuffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        colour_attachments_.resize(num_fbuffer_attachments);
        colour_attachments_[fbuffer_composite] = std::make_shared<texture_2d>("composite", _width, _height, sized_format::rgba8);
        colour_attachments_[fbuffer_position] = std::make_shared<texture_2d>("diffuse", _width, _height, sized_format::rgba8);
        colour_attachments_[fbuffer_normal] = std::make_shared<texture_2d>("specular", _width, _height, sized_format::rgba8);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Depth-Stencil attachments.
        depth_stencil_attachment_ = std::make_shared<texture_2d>("depth_stencil", _width, _height, sized_format::depth24_stencil8);
        glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, depth_stencil_attachment_->handle(), 0);

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete forward buffer");
        }
    }

    fbuffer::~fbuffer() {
        glDeleteFramebuffers(1, &handle_);
    }

    // Post-Process Buffer
    pbuffer::pbuffer(uint32_t _width, uint32_t _height) {
        // Create GL buffer.
        glCreateFramebuffers(1, &handle_);

        // Colour attachments.
        colour_attachments_.resize(num_pbuffer_attachments);
        colour_attachments_[pbuffer_composite] = std::make_shared<texture_2d>("composite", _width, _height, sized_format::rgba8);
        for (auto i = 0; i < colour_attachments_.size(); ++i) {
            glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + i, colour_attachments_[i]->handle(), 0);
        }

        // Completeness check.
        if (!is_complete()) {
            throw std::runtime_error("incomplete forward buffer");
        }
    }

    pbuffer::~pbuffer() {
        glDeleteFramebuffers(1, &handle_);
    }
}