#pragma once

#include <array>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <maths/maths_util.h>
#include "graphics/texture/pixel_format.h"

namespace mkr {
    class texture {
    protected:
        const std::string name_;
        const uint32_t width_, height_;
        GLuint handle_;

        static uint32_t mip_map_level(uint32_t _width, uint32_t _height) {
            const uint32_t log2_width = static_cast<uint32_t>(std::log2f(static_cast<float>(_width)));
            const uint32_t log2_height = static_cast<uint32_t>(std::log2f(static_cast<float>(_height)));
            return maths_util::clamp<uint32_t>(maths_util::min(log2_width, log2_height), 1, GL_TEXTURE_MAX_LEVEL);
        }

        texture(const std::string& _name, uint32_t _width, uint32_t _height)
            : handle_(0), name_(_name), width_(_width), height_(_height) {}

    public:
        virtual ~texture() {}

        [[nodiscard]] inline const std::string& name() const { return name_; }

        [[nodiscard]] inline uint32_t width() const { return width_; }

        [[nodiscard]] inline uint32_t height() const { return height_; }

        [[nodiscard]] inline GLuint handle() const { return handle_; }

        inline void bind(uint32_t _texture_unit) { glBindTextureUnit((GLuint) _texture_unit, handle_); }
    };

    class texture2d : public texture {
    public:
        // Image
        texture2d(const std::string& _name, uint32_t _width, uint32_t _height, const void* _data, sized_format _internal_format = sized_format::rgba8)
            : texture(_name, _width, _height) {
            /**
             * IMPORTANT: Unlike glTexImage2D, we have to explicitly state the number of mipmaps to generate.
             * We can no longer leave it at 0 and expect OpenGL to figure out how many mipmaps to generate.
             * This is because glTextureStorage2D creates a fixed storage space for our texture, not like
             * glTexImage2D where it can resize itself to generate mipmaps. The glTextureStorage2D documentation
             * states "GL_INVALID_VALUE is generated if width, height or levels are less than 1." */
            // Direct-State-Access Method (OpenGL 4.5) Create Texture
            glCreateTextures(GL_TEXTURE_2D, 1, &handle_);

            glTextureStorage2D(handle_, (GLsizei) mip_map_level(width_, height_),
                               (GLenum) _internal_format, ///< Format to store the texture data in OpenGL.
                               (GLsizei) width_, (GLsizei) height_);
            glTextureSubImage2D(handle_, 0, 0, 0, (GLsizei) width_, (GLsizei) height_,
                                (GLenum) pixel_format::sized_to_base(_internal_format), ///< Format of the image data being passed in. It is expected to be compatible with the sized format.
                                GL_UNSIGNED_BYTE, _data);

            // Texture Parameter(s)
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Generate Mipmap
            glGenerateTextureMipmap(handle_);
        }

        // Framebuffer attachment.
        texture2d(const std::string& _name, uint32_t _width, uint32_t _height, sized_format _internal_format)
            : texture(_name, _width, _height) {
            glCreateTextures(GL_TEXTURE_2D, 1, &handle_);
            glTextureStorage2D(handle_, 1, (GLenum) _internal_format, (GLsizei) width_, (GLsizei) height_);
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glGenerateTextureMipmap(handle_);
        }

        virtual ~texture2d() {
            // Delete Texture
            glDeleteTextures(1, &handle_);
        }
    };

    /**
     * [https://www.khronos.org/opengl/wiki/Cubemap_Texture]
     * [https://stackoverflow.com/questions/11685608/convention-of-faces-in-opengl-cubemapping]
     *
     * !!!Important!!!
     * Due to some stupid decisions by OpenGL to follow Pixar's RenderMan convention,
     * 1. Cubemap's coordinate system is left-handed, instead of right-handed.
     * 2. Cubemap's texture coordinates go from top-left to bottom-right, instead of bottom-left to top-right.
     *
     * We order the sides of the cubemap according to OpenGL's cubemap face order.
     * Layer 0 - GL_TEXTURE_CUBE_MAP_POSITIVE_X
     * Layer 1 - GL_TEXTURE_CUBE_MAP_NEGATIVE_X
     * Layer 2 - GL_TEXTURE_CUBE_MAP_POSITIVE_Y
     * Layer 3 - GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
     * Layer 4 - GL_TEXTURE_CUBE_MAP_POSITIVE_Z
     * Layer 5 - GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
     */
    enum cubemap_side {
        cubemap_positive_x = 0,
        cubemap_negative_x = 1,
        cubemap_positive_y = 2,
        cubemap_negative_y = 3,
        cubemap_positive_z = 4,
        cubemap_negative_z = 5,

        // This is the convention I decided on.
        cubemap_right = cubemap_positive_x,
        cubemap_left = cubemap_negative_x,
        cubemap_top = cubemap_positive_y,
        cubemap_bottom = cubemap_negative_y,
        cubemap_front = cubemap_positive_z,
        cubemap_back = cubemap_negative_z,

        num_cubemap_sides = 6,
    };

    class cubemap : public texture {
    public:
        cubemap(const std::string& _name, uint32_t _size, std::array<const void*, num_cubemap_sides> _data, sized_format _internal_format = sized_format::rgba8)
            : texture(_name, _size, _size) {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &handle_);

            glTextureStorage2D(handle_, (GLsizei) mip_map_level(width_, height_),
                               (GLenum) _internal_format, ///< Format to store the texture data in OpenGL.
                               (GLsizei) width_, (GLsizei) height_);

            // Note that for cubemaps, we have to use glTextureSubImage3D instead of glTextureSubImage2D, but we still use glTextureStorage2D.
            for (auto i = 0; i < num_cubemap_sides; ++i) {
                glTextureSubImage3D(handle_, 0, 0, 0, i, (GLsizei) width_, (GLsizei) height_, 1,
                                    (GLenum) pixel_format::sized_to_base(_internal_format), ///< Format of the image data being passed in. It is expected to be compatible with the sized format.
                                    GL_UNSIGNED_BYTE, _data[i]);
            }
            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glGenerateTextureMipmap(handle_);
        }

        // Framebuffer attachment.
        cubemap(const std::string& _name, uint32_t _size, sized_format _internal_format = sized_format::rgba8)
            : texture(_name, _size, _size) {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &handle_);
            glTextureStorage2D(handle_, 1, (GLenum) _internal_format, (GLsizei) width_, (GLsizei) height_);
            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glGenerateTextureMipmap(handle_);
        }

        virtual ~cubemap() {
            glDeleteTextures(1, &handle_);
        }
    };
}