#pragma once

#include <cmath>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>

namespace mkr {
    enum texture_unit {
        texture_skybox,
        texture_albedo,
    };

    enum texture_wrap_mode : GLint {
        repeat = GL_REPEAT,
        mirrored_repeat = GL_MIRRORED_REPEAT,
        clamp_to_edge = GL_CLAMP_TO_EDGE,
    };

    /**
     * [https://www.khronos.org/opengl/wiki/Cubemap_Texture]
     *
     * Important: OpenGL uses left-hand coordinate system for cubemap orientations,
     * as opposed to the usual right-hand coordinate system for everything else.
     *
     * We order the sides of the cubemap according to OpenGL's cubemap face order.
     * Layer 0 - GL_TEXTURE_CUBE_MAP_POSITIVE_X
     * Layer 1 - GL_TEXTURE_CUBE_MAP_NEGATIVE_X
     * Layer 2 - GL_TEXTURE_CUBE_MAP_POSITIVE_Y
     * Layer 3 - GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
     * Layer 4 - GL_TEXTURE_CUBE_MAP_POSITIVE_Z
     * Layer 5 - GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
     */
    enum texture_cube_side {
        texture_cube_positive_x = 0,
        texture_cube_negative_X = 1,
        texture_cube_positive_Y = 2,
        texture_cube_negative_Y = 3,
        texture_cube_positive_Z = 4,
        texture_cube_negative_Z = 5,

        // We take positive y to be forward, negative x to be left, and positive y to be up.
        texture_cube_right = texture_cube_positive_x,
        texture_cube_left = texture_cube_negative_X,
        texture_cube_top = texture_cube_positive_Y,
        texture_cube_bottom = texture_cube_negative_Y,
        texture_cube_front = texture_cube_positive_Z,
        texture_cube_back = texture_cube_negative_Z,

        num_texture_cube_sides = 6,
    };

    class texture {
    protected:
        const std::string name_;
        const uint32_t width_, height_;
        GLuint handle_;
        texture_wrap_mode wrap_mode_;

        static uint32_t mip_map_level(uint32_t _width, uint32_t _height) {
            const uint32_t log2_width = static_cast<uint32_t>(std::log2f(static_cast<float>(_width)));
            const uint32_t log2_height = static_cast<uint32_t>(std::log2f(static_cast<float>(_height)));
            return maths_util::clamp<uint32_t>(maths_util::min(log2_width, log2_height), 1, GL_TEXTURE_MAX_LEVEL);
        }

        texture(const std::string& _name, uint32_t _width, uint32_t _height, texture_wrap_mode _wrap_mode)
                : handle_(0), name_(_name), width_(_width), height_(_height), wrap_mode_(_wrap_mode) {}

    public:
        virtual ~texture() {}

        [[nodiscard]] inline const std::string& name() const { return name_; }

        [[nodiscard]] inline uint32_t width() const { return width_; }

        [[nodiscard]] inline uint32_t height() const { return height_; }

        [[nodiscard]] inline texture_wrap_mode wrap_mode() const { return wrap_mode_; }

        inline void bind(texture_unit _texture_unit) { glBindTextureUnit(_texture_unit, handle_); }
    };

    class texture_2d : public texture {
    public:
        texture_2d(const std::string& _name, uint32_t _width, uint32_t _height, const void* _data)
                : texture(_name, _width, _height, texture_wrap_mode::repeat) {
            /**
             * IMPORTANT: Unlike glTexImage2D, we have to explicitly state the number of mipmaps to generate.
             * We can no longer leave it at 0 and expect OpenGL to figure out how many mipmaps to generate.
             * This is because glTextureStorage2D creates a fixed storage space for our texture, not like
             * glTexImage2D where it can resize itself to generate mipmaps. The glTextureStorage2D documentation
             * states "GL_INVALID_VALUE is generated if width, height or levels are less than 1." */
            // Direct-State-Access Method (OpenGL 4.5) Create Texture
            glCreateTextures(GL_TEXTURE_2D, 1, &handle_);

            glTextureStorage2D(handle_, (GLsizei)mip_map_level(width_, height_),
                               GL_RGBA8, ///< Format to store the texture data in OpenGL.
                               (GLsizei)width_, (GLsizei)height_);
            glTextureSubImage2D(handle_, 0, 0, 0, (GLsizei)width_, (GLsizei)height_,
                                GL_RGBA, ///< Format of the image data.
                                GL_UNSIGNED_BYTE, _data);

            // Texture Parameter(s)
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, wrap_mode_);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, wrap_mode_);

            // Generate Mipmap
            glGenerateTextureMipmap(handle_);
        }

        virtual ~texture_2d() {
            // Delete Texture
            glDeleteTextures(1, &handle_);
        }
    };

    class texture_cube : public texture {
    public:
        texture_cube(const std::string& _name, uint32_t _width, uint32_t _height, std::array<const void*, num_texture_cube_sides> _data)
                : texture(_name, _width, _height, texture_wrap_mode::clamp_to_edge) {
            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &handle_);

            glTextureStorage2D(handle_, (GLsizei)mip_map_level(width_, height_),
                               GL_RGBA8, ///< Format to store the texture data in OpenGL. Standardise to RGBA8
                               (GLsizei)width_, (GLsizei)height_);

            // Note that for cubemaps, we have to use glTextureSubImage3D instead of glTextureSubImage2D, but we still use glTextureStorage2D.
            for (auto i = 0; i < num_texture_cube_sides; ++i) {
                glTextureSubImage3D(handle_, 0, 0, 0, i, (GLsizei)width_, (GLsizei)height_, 1,
                                    GL_RGBA, ///< Format of the image data.
                                    GL_UNSIGNED_BYTE, _data[i]);
            }
            GL_TEXTURE_CUBE_MAP_POSITIVE_X;

            glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(handle_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glGenerateTextureMipmap(handle_);
        }

        virtual ~texture_cube() {
            glDeleteTextures(1, &handle_);
        }
    };
}