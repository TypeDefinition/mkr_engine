#include "graphics/material.h"

namespace mkr {
    bool material::enable_lights_ = true;
    std::shared_ptr<shader_program> material::g_shader_;
    std::shared_ptr<shader_program> material::l_shader_;
    std::vector<std::shared_ptr<shader_program>> material::p_shaders_;
}