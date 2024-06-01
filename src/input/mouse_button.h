#pragma once

#include "input/input.h"

namespace mkr {
// Keyboard Button Range: [300, 400)
enum mouse_button : keycode_t {
    mouse_invalid = 300,

    mouse_left,
    mouse_right,
    mouse_middle,
    mouse_x1,
    mouse_x2,
};
} // mkr