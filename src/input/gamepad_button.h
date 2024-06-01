#pragma once

#include "input/input.h"

namespace mkr {
    // Gamepad Button Range: [400, 500)
    enum gamepad_button : keycode_t {
        gamepad_button_invalid = 400,

        gamepad_button_a,
        gamepad_button_b,
        gamepad_button_x,
        gamepad_button_y,
        gamepad_button_back,
        gamepad_button_guide,
        gamepad_button_start,
        gamepad_button_lstick,
        gamepad_button_rstick,
        gamepad_button_lshoulder,
        gamepad_button_rshoulder,
        gamepad_button_dpad_up,
        gamepad_button_dpad_down,
        gamepad_button_dpad_left,
        gamepad_button_dpad_right,
        gamepad_button_misc1,
        gamepad_button_paddle1,
        gamepad_button_paddle2,
        gamepad_button_paddle3,
        gamepad_button_paddle4,
        gamepad_button_touchpad,
    };
} // mkr
