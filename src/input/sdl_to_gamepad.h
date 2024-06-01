#pragma once

#include "SDL2/SDL_gamecontroller.h"
#include "input/gamepad_button.h"
#include "input/gamepad_axis.h"

namespace mkr {
class sdl_to_gamepad {
public:
    sdl_to_gamepad() = delete;

    static gamepad_button to_button(SDL_GameControllerButton _sdl) {
        switch (_sdl) {
        case SDL_CONTROLLER_BUTTON_A: return gamepad_button_a;
        case SDL_CONTROLLER_BUTTON_B: return gamepad_button_b;
        case SDL_CONTROLLER_BUTTON_X: return gamepad_button_x;
        case SDL_CONTROLLER_BUTTON_Y: return gamepad_button_y;
        case SDL_CONTROLLER_BUTTON_BACK: return gamepad_button_back;
        case SDL_CONTROLLER_BUTTON_GUIDE: return gamepad_button_guide;
        case SDL_CONTROLLER_BUTTON_START: return gamepad_button_start;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return gamepad_button_lstick;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return gamepad_button_rstick;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return gamepad_button_lshoulder;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return gamepad_button_rshoulder;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return gamepad_button_dpad_up;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return gamepad_button_dpad_down;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return gamepad_button_dpad_left;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return gamepad_button_dpad_right;
        case SDL_CONTROLLER_BUTTON_MISC1: return gamepad_button_misc1;
        // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button.
        case SDL_CONTROLLER_BUTTON_PADDLE1: return gamepad_button_paddle1; // Xbox Elite paddle P1.
        case SDL_CONTROLLER_BUTTON_PADDLE2: return gamepad_button_paddle2; // Xbox Elite paddle P3.
        case SDL_CONTROLLER_BUTTON_PADDLE3: return gamepad_button_paddle3; // Xbox Elite paddle P2.
        case SDL_CONTROLLER_BUTTON_PADDLE4: return gamepad_button_paddle4; // Xbox Elite paddle P4.
        case SDL_CONTROLLER_BUTTON_TOUCHPAD: return gamepad_button_touchpad; // PS5 touchpad button.
        }
        return gamepad_button_invalid;
    }

    static gamepad_axis to_axis(SDL_GameControllerAxis _sdl) {
        switch (_sdl) {
        case SDL_CONTROLLER_AXIS_LEFTX: return gamepad_axis_left_x;
        case SDL_CONTROLLER_AXIS_LEFTY: return gamepad_axis_left_y;
        case SDL_CONTROLLER_AXIS_RIGHTX: return gamepad_axis_right_x;
        case SDL_CONTROLLER_AXIS_RIGHTY: return gamepad_axis_right_y;
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return gamepad_axis_trigger_left;
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return gamepad_axis_trigger_right;
        }
        return gamepad_axis_invalid;
    }

    static controller_index to_controller_index(SDL_JoystickID _sdl) {
        switch (_sdl) {
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
        }
        return controller_index_none;
    }
};
} // mkr
