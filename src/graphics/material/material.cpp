#include "graphics/material/material.h"

namespace mkr {
    shader_program* material::geometry_shader_;
    shader_program* material::light_shader_;
    std::vector<shader_program*> material::post_proc_shaders_;
}