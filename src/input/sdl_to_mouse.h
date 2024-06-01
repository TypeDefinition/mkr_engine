#pragma once

#include <SDL2/SDL_mouse.h>
#include "input/mouse_button.h"

namespace mkr {
class sdl_to_mouse {
public:
    sdl_to_mouse() = delete;

    static mouse_button to_button(int32_t _sdl) {
        switch (_sdl) {
        case SDL_BUTTON_LEFT: return mouse_left;
        case SDL_BUTTON_MIDDLE: return mouse_middle;
        case SDL_BUTTON_RIGHT: return mouse_right;
        case SDL_BUTTON_X1: return mouse_x1;
        case SDL_BUTTON_X2: return mouse_x2;
        }
        return mouse_invalid;
    }
};
}
