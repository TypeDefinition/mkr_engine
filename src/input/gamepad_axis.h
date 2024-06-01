#pragma once

#include "input/input.h"

namespace mkr {
    // Gamepad Axis Range: [100, 200)
    enum gamepad_axis : keycode_t {
        gamepad_axis_invalid = 100,

        gamepad_axis_left_x,
        gamepad_axis_left_y,
        gamepad_axis_right_x,
        gamepad_axis_right_y,
        gamepad_axis_trigger_left,
        gamepad_axis_trigger_right,
    };
}
