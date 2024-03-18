#include "graphics/shader/post_proc_shader.h"
#include "graphics/shader/texture_unit.h"

namespace mkr {
    post_proc_shader::post_proc_shader(const std::string& _name, const std::vector<std::string>& _vs_sources, const std::vector<std::string>& _fs_sources)
        : shader_program(_name, _vs_sources, _fs_sources, uniform::num_shader_uniforms) {
        assign_uniforms();
        assign_textures();
    }

    void post_proc_shader::assign_uniforms() {
        // Fragment Shader
        uniform_handles_[uniform::u_near] = get_uniform_location("u_near");
        uniform_handles_[uniform::u_far] = get_uniform_location("u_far");
        uniform_handles_[uniform::u_bottom_left] = get_uniform_location("u_bottom_left");
        uniform_handles_[uniform::u_top_right] = get_uniform_location("u_top_right");

        uniform_handles_[uniform::u_texture_frag_position] = get_uniform_location("u_texture_frag_position");
        uniform_handles_[uniform::u_texture_frag_normal] = get_uniform_location("u_texture_frag_normal");
        uniform_handles_[uniform::u_texture_depth_stencil] = get_uniform_location("u_texture_depth_stencil");
        uniform_handles_[uniform::u_texture_composite] = get_uniform_location("u_texture_composite");
    }

    void post_proc_shader::assign_textures() {
        // set_uniform(uniform::u_texture_frag_position, (int32_t)texture_unit::texture_frag_position);
        // set_uniform(uniform::u_texture_frag_normal, (int32_t)texture_unit::texture_frag_normal);
        // set_uniform(uniform::u_texture_depth_stencil, (int32_t)texture_unit::texture_depth_stencil);
        // set_uniform(uniform::u_texture_composite, (int32_t)texture_unit::texture_composite);
    }
} // mkr