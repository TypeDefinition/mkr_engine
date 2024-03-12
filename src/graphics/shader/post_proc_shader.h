#pragma once

#include "graphics/shader/shader_program.h"

namespace mkr {
    class post_proc_shader : public shader_program {
    public:
        enum uniform : uint32_t {
            u_near,
            u_far,
            u_bottom_left,
            u_top_right,

            u_texture_frag_position,
            u_texture_frag_normal,
            u_texture_depth_stencil,
            u_texture_composite,

            num_shader_uniforms,
        };

    protected:
        /**
         * Assign uniforms to uniform_handles_.
         */
        void assign_uniforms();

        /**
         * Assign textures to GL_TEXTURE0 to GL_TEXTUREN.
         * For some weird reason, glProgramUniform1i and glProgramUniform1iv are the only two functions
         * that may be used to load uniform variables defined as sampler types.
         * Loading samplers with any other function will result in a GL_INVALID_OPERATION error.
         * Thus, we must cast texture_unit to a signed integer.
         */
        void assign_textures();

    public:
        post_proc_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources);

        virtual ~post_proc_shader() {}
    };
} // mkr