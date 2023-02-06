#pragma once

#include <stdexcept>
#include <GL/glew.h>

namespace mkr {
    enum class sized_format : GLenum {
        // Colour
        r8 = GL_R8,
        r8_snorm = GL_R8_SNORM,
        r16 = GL_R16,
        r16_snorm = GL_R16_SNORM,
        rg8 = GL_RG8,
        rg8_snorm = GL_RG8_SNORM,
        rg16 = GL_RG16,
        rg16_snorm = GL_RG16_SNORM,
        r3_g3_b2 = GL_R3_G3_B2,
        rgb4 = GL_RGB4,
        rgb5 = GL_RGB5,
        rgb8 = GL_RGB8,
        rgb8_snorm = GL_RGB8_SNORM,
        rgb10 = GL_RGB10,
        rgb12 = GL_RGB12,
        rgb16_snorm = GL_RGB16_SNORM,
        rgba2 = GL_RGBA2,
        rgba4 = GL_RGBA4,
        rgb5_a1 = GL_RGB5_A1,
        rgba8 = GL_RGBA8,
        rgba8_snorm = GL_RGBA8_SNORM,
        rgb10_a2 = GL_RGB10_A2,
        rgb10_a2ui = GL_RGB10_A2UI,
        rgba12 = GL_RGBA12,
        rgba16 = GL_RGBA16,
        srgb8 = GL_SRGB8,
        srgb8_alpha8 = GL_SRGB8_ALPHA8,
        r16f = GL_R16F,
        rg16f = GL_RG16F,
        rgb16f = GL_RGB16F,
        rgba16f = GL_RGBA16F,
        r32f = GL_R32F,
        rg32f = GL_RG32F,
        rgb32f = GL_RGB32F,
        rgba32f = GL_RGBA32F,
        r11f_g11f_b10f = GL_R11F_G11F_B10F,
        rgb9_e5 = GL_RGB9_E5,
        r8i = GL_R8I,
        r8ui = GL_R8UI,
        r16i = GL_R16I,
        r16ui = GL_R16UI,
        r32i = GL_R32I,
        r32ui = GL_R32UI,
        rg8i = GL_RG8I,
        rg8ui = GL_RG8UI,
        rg16i = GL_RG16I,
        rg16ui = GL_RG16UI,
        rg32i = GL_RG32I,
        rg32ui = GL_RG32UI,
        rgb8i = GL_RGB8I,
        rgb8ui = GL_RGB8UI,
        rgb16i = GL_RGB16I,
        rgb16ui = GL_RGB16UI,
        rgb32i = GL_RGB32I,
        rgb32ui = GL_RGB32UI,
        rgba8i = GL_RGBA8I,
        rgba8ui = GL_RGBA8UI,
        rgba16i = GL_RGBA16I,
        rgba16ui = GL_RGBA16UI,
        rgba32i = GL_RGBA32I,
        rgba32ui = GL_RGBA32UI,

        // Depth-Stencil
        depth_component16 = GL_DEPTH_COMPONENT16,
        depth_component24 = GL_DEPTH_COMPONENT24,
        depth_component32 = GL_DEPTH_COMPONENT32,
        depth_component32f = GL_DEPTH_COMPONENT32F,
        depth24_stencil8 = GL_DEPTH24_STENCIL8,
        depth32f_stencil8 = GL_DEPTH32F_STENCIL8,
        stencil_index1 = GL_STENCIL_INDEX1,
        stencil_index4 = GL_STENCIL_INDEX4,
        stencil_index8 = GL_STENCIL_INDEX8,
        stencil_index16 = GL_STENCIL_INDEX16,
    };

    enum class base_format : GLenum {
        // Colour
        red = GL_RED,
        rg = GL_RG,
        rgb = GL_RGB,
        rgba = GL_RGBA,

        // Depth-Stencil
        depth_component = GL_DEPTH_COMPONENT,
        depth_stencil = GL_DEPTH_STENCIL,
        stencil_index =  GL_STENCIL_INDEX,
    };

    class pixel_format {
    public:
        pixel_format() = delete;

        static base_format sized_to_base(sized_format _format) {
            switch (_format) {
                // Colour
                case sized_format::r8:
                case sized_format::r8_snorm:
                case sized_format::r16:
                case sized_format::r16_snorm:
                    return base_format::red;
                case sized_format::rg8:
                case sized_format::rg8_snorm:
                case sized_format::rg16:
                case sized_format::rg16_snorm:
                    return base_format::rg;
                case sized_format::r3_g3_b2:
                case sized_format::rgb4:
                case sized_format::rgb5:
                case sized_format::rgb8:
                case sized_format::rgb8_snorm:
                case sized_format::rgb10:
                case sized_format::rgb12:
                case sized_format::rgb16_snorm:
                    return base_format::rgb;
                case sized_format::rgba2:
                case sized_format::rgba4:
                case sized_format::rgb5_a1:
                case sized_format::rgba8:
                case sized_format::rgba8_snorm:
                case sized_format::rgb10_a2:
                case sized_format::rgb10_a2ui:
                case sized_format::rgba12:
                case sized_format::rgba16:
                    return base_format::rgba;
                case sized_format::srgb8:
                    return base_format::rgb;
                case sized_format::srgb8_alpha8:
                    return base_format::rgba;
                case sized_format::r16f:
                    return base_format::red;
                case sized_format::rg16f:
                    return base_format::rg;
                case sized_format::rgb16f:
                    return base_format::rgb;
                case sized_format::rgba16f:
                    return base_format::rgba;
                case sized_format::r32f:
                    return base_format::red;
                case sized_format::rg32f:
                    return base_format::rg;
                case sized_format::rgb32f:
                    return base_format::rgb;
                case sized_format::rgba32f:
                    return base_format::rgba;
                case sized_format::r11f_g11f_b10f:
                case sized_format::rgb9_e5:
                    return base_format::rgb;
                case sized_format::r8i:
                case sized_format::r8ui:
                case sized_format::r16i:
                case sized_format::r16ui:
                case sized_format::r32i:
                case sized_format::r32ui:
                case sized_format::rg8i:
                    return base_format::red;
                case sized_format::rg8ui:
                case sized_format::rg16i:
                case sized_format::rg16ui:
                case sized_format::rg32i:
                case sized_format::rg32ui:
                    return base_format::rg;
                case sized_format::rgb8i:
                case sized_format::rgb8ui:
                case sized_format::rgb16i:
                case sized_format::rgb16ui:
                case sized_format::rgb32i:
                case sized_format::rgb32ui:
                    return base_format::rgb;
                case sized_format::rgba8i:
                case sized_format::rgba8ui:
                case sized_format::rgba16i:
                case sized_format::rgba16ui:
                case sized_format::rgba32i:
                case sized_format::rgba32ui:
                    return base_format::rgba;

                // Depth-Stencil
                case sized_format::depth_component16:
                case sized_format::depth_component24:
                case sized_format::depth_component32:
                case sized_format::depth_component32f:
                    return base_format::depth_component;
                case sized_format::depth24_stencil8:
                case sized_format::depth32f_stencil8:
                    return base_format::depth_stencil;
                case sized_format::stencil_index1:
                case sized_format::stencil_index4:
                case sized_format::stencil_index8:
                case sized_format::stencil_index16:
                    return base_format::stencil_index;

                default:
                    throw std::runtime_error("invalid sized format");
                    return base_format::rgba;
            };
        }
    };
}