#pragma once

#include "input/input.h"

namespace mkr {
// Mouse Axis Range: [0, 100)
enum mouse_axis : keycode_t {
    mouse_axis_invalid = 0,

    mouse_axis_x,
    mouse_axis_y,
};
} // mkr