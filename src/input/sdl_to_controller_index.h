#pragma once

#include <cstdint>
#include <SDL2/SDL_joystick.h>
#include "input/input.h"

namespace mkr {
    class sdl_to_controller_index {
    public:
        sdl_to_controller_index() = delete;

        static controller_index from_joystick_id(SDL_JoystickID _joystick_id) {
            switch (_joystick_id) {
                case 0: return controller_index_0;
                case 1: return controller_index_1;
                case 2: return controller_index_2;
                case 3: return controller_index_3;
                case 4: return controller_index_4;
                case 5: return controller_index_5;
                case 6: return controller_index_6;
                case 7: return controller_index_7;
                case 8: return controller_index_8;
                case 9: return controller_index_9;
                case 10: return controller_index_10;
                case 11: return controller_index_11;
                case 12: return controller_index_12;
                case 13: return controller_index_13;
                case 14: return controller_index_14;
                case 15: return controller_index_15;
                default: return controller_index_none;
            }
        }
    };
} // mkr