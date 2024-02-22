#pragma once

#include "input/input.h"

namespace mkr {
    enum game_controls : input_action {
        quit = 0,

        move_left,
        move_right,
        move_forward,
        move_backward,

        look_left,
        look_right,
        look_up,
        look_down,

        debug_mode_off,
        debug_mode_position,
        debug_mode_normal,
        debug_mode_albedo,
        debug_mode_specular,
        debug_mode_gloss,
        debug_mode_material,
    };
}