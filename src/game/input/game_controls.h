#pragma once

#include "input/input.h"

namespace mkr {
    enum game_controls : input_action_t {
        // Buttons
        quit = 0,

        move_left,
        move_right,
        move_forward,
        move_backward,

        look_left,
        look_right,
        look_up,
        look_down,

        look_horizontal,
        look_vertical,

        // Clicks
        test_click,

        // Motions
        test_motion,
    };
} // mkr