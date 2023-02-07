#include "graphics/material.h"

namespace mkr {
    bool material::enable_lights_ = true;
    colour material::ambient_colour_ = colour::constexpr_dark_grey();
    std::shared_ptr<shader_program> material::g_shader_;
    std::shared_ptr<shader_program> material::l_shader_;
    std::vector<std::shared_ptr<shader_program>> material::p_shaders_;
}