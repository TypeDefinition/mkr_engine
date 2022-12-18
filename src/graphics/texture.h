#pragma once

#include <cmath>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

namespace mkr {
    enum texture_unit : GLuint {
        texture_albedo,
    };

    enum texture_wrap_mode : GLint {
        repeat = GL_REPEAT,
        mirrored_repeat = GL_MIRRORED_REPEAT,
        clamp_to_edge = GL_CLAMP_TO_EDGE,
    };

    enum texture_shape : GLenum {
        shape_2d = GL_TEXTURE_2D,
        shape_cube = GL_TEXTURE_CUBE_MAP,
    };

    class texture {
    protected:
        GLuint handle_;
        const std::string name_;
        uint32_t width_, height_;
        texture_shape shape_;
        texture_wrap_mode wrap_mode_;

        static uint32_t mip_map_level(uint32_t _width, uint32_t _height) {
            const uint32_t log2_width = static_cast<uint32_t>(std::log2f(static_cast<float>(_width)));
            const uint32_t log2_height = static_cast<uint32_t>(std::log2f(static_cast<float>(_height)));
            return maths_util::clamp<uint32_t>(maths_util::min(log2_width, log2_height), 1, GL_TEXTURE_MAX_LEVEL);
        }

        texture(const std::string &_name, uint32_t _width, uint32_t _height, texture_shape _shape, texture_wrap_mode _wrap_mode)
                : handle_(0), name_(_name), width_(_width), height_(_height), shape_(_shape), wrap_mode_(_wrap_mode) {}

    public:
        virtual ~texture() {}

        inline const std::string &name() const { return name_; }

        inline uint32_t width() const { return width_; }

        inline uint32_t height() const { return height_; }

        inline texture_shape shape() const { return shape_; }

        inline texture_wrap_mode wrap_mode() const { return wrap_mode_; }

        inline void bind(texture_unit _texture_unit) { glBindTextureUnit(_texture_unit, handle_); }
    };

    class texture_2d : public texture {
    public:
        texture_2d(const std::string &_name, uint32_t _width, uint32_t _height, const void *_data)
                : texture(_name, _width, _height, texture_shape::shape_2d, texture_wrap_mode::repeat) {
            /**
             * IMPORTANT: Unlike glTexImage2D, we have to explicitly state the number of mipmaps to generate.
             * We can no longer leave it at 0 and expect OpenGL to figure out how many mipmaps to generate.
             * This is because glTextureStorage2D creates a fixed storage space for our texture, not like
             * glTexImage2D where it can resize itself to generate mipmaps. The glTextureStorage2D documentation
             * states "GL_INVALID_VALUE is generated if width, height or levels are less than 1." */

            glCreateTextures(GL_TEXTURE_2D, 1, &handle_);

            glTextureStorage2D(handle_, mip_map_level(width_, height_),
                               GL_RGBA8, ///< Format to store the texture data in OpenGL.
                               width_, height_);
            glTextureSubImage2D(handle_, 0, 0, 0, width_, height_,
                                GL_RGBA, ///< Format of the image data.
                                GL_UNSIGNED_BYTE, _data);

            // Texture Parameter(s)
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, wrap_mode_);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, wrap_mode_);

            glGenerateTextureMipmap(handle_);
        }

        virtual ~texture_2d() {}
    };

    class texture_cube : public texture {
    public:
        texture_cube(const std::string &_name, uint32_t _width, uint32_t _height, std::array<const void *, 6> _data)
                : texture(_name, _width, _height, texture_shape::shape_2d, texture_wrap_mode::clamp_to_edge) {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &handle_);
        }

        virtual ~texture_cube() {}
    };
}