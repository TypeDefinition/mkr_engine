#include "graphics/material.h"

namespace mkr {
    bool material::enable_lights_ = true;
    colour material::ambient_colour_ = colour::constexpr_dark_grey();
    std::shared_ptr<shader_program> material::gshader_;
    std::shared_ptr<shader_program> material::lshader_;
    std::shared_ptr<shader_program> material::sshader_;
    std::vector<std::shared_ptr<shader_program>> material::pshaders_;
}