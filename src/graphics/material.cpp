#include "graphics/material.h"

namespace mkr {
    bool material::enable_lights_ = true;
    colour material::ambient_colour_ = colour::constexpr_dark_grey();
    shader_program* material::gshader_;
    shader_program* material::lshader_;
    shader_program* material::sshader_;
    std::vector<shader_program*> material::pshaders_;
}