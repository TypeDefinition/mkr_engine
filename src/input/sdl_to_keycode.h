#pragma once

#include <cstdint>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include "SDL2/SDL_gamecontroller.h"
#include "input/keycode.h"

namespace mkr {
    class sdl_to_keycode {
    public:
        sdl_to_keycode() = delete;

        static keycode from_keyboard_button(SDL_KeyCode _sdl) {
            switch (_sdl) {
                case SDLK_UNKNOWN: return kc_unknown;
                case SDLK_RETURN: return kc_return;
                case SDLK_ESCAPE: return kc_escape;
                case SDLK_BACKSPACE: return kc_backspace;
                case SDLK_TAB: return kc_tab;
                case SDLK_SPACE: return kc_space;
                case SDLK_EXCLAIM: return kc_exclaim;
                case SDLK_QUOTEDBL: return kc_quote_dbl;
                case SDLK_HASH: return kc_hash;
                case SDLK_PERCENT: return kc_percent;
                case SDLK_DOLLAR: return kc_dollar;
                case SDLK_AMPERSAND: return kc_ampersand;
                case SDLK_QUOTE: return kc_quote;
                case SDLK_LEFTPAREN: return kc_left_paren;
                case SDLK_RIGHTPAREN: return kc_right_paren;
                case SDLK_ASTERISK: return kc_asterisk;
                case SDLK_PLUS: return kc_plus;
                case SDLK_COMMA: return kc_comma;
                case SDLK_MINUS: return kc_minus;
                case SDLK_PERIOD: return kc_period;
                case SDLK_SLASH: return kc_slash;
                case SDLK_0: return kc_0;
                case SDLK_1: return kc_1;
                case SDLK_2: return kc_2;
                case SDLK_3: return kc_3;
                case SDLK_4: return kc_4;
                case SDLK_5: return kc_5;
                case SDLK_6: return kc_6;
                case SDLK_7: return kc_7;
                case SDLK_8: return kc_8;
                case SDLK_9: return kc_9;
                case SDLK_COLON: return kc_colon;
                case SDLK_SEMICOLON: return kc_semicolon;
                case SDLK_LESS: return kc_less;
                case SDLK_EQUALS: return kc_equals;
                case SDLK_GREATER: return kc_greater;
                case SDLK_QUESTION: return kc_question;
                case SDLK_AT: return kc_at;

                case SDLK_LEFTBRACKET: return kc_left_bracket;
                case SDLK_BACKSLASH: return kc_backslash;
                case SDLK_RIGHTBRACKET: return kc_right_bracket;
                case SDLK_CARET: return kc_caret;
                case SDLK_UNDERSCORE: return kc_underscore;
                case SDLK_BACKQUOTE: return kc_backquote;
                case SDLK_a: return kc_a;
                case SDLK_b: return kc_b;
                case SDLK_c: return kc_c;
                case SDLK_d: return kc_d;
                case SDLK_e: return kc_e;
                case SDLK_f: return kc_f;
                case SDLK_g: return kc_g;
                case SDLK_h: return kc_h;
                case SDLK_i: return kc_i;
                case SDLK_j: return kc_j;
                case SDLK_k: return kc_k;
                case SDLK_l: return kc_l;
                case SDLK_m: return kc_m;
                case SDLK_n: return kc_n;
                case SDLK_o: return kc_o;
                case SDLK_p: return kc_p;
                case SDLK_q: return kc_q;
                case SDLK_r: return kc_r;
                case SDLK_s: return kc_s;
                case SDLK_t: return kc_t;
                case SDLK_u: return kc_u;
                case SDLK_v: return kc_v;
                case SDLK_w: return kc_w;
                case SDLK_x: return kc_x;
                case SDLK_y: return kc_y;
                case SDLK_z: return kc_z;

                case SDLK_CAPSLOCK: return kc_capslock;

                case SDLK_F1: return kc_f1;
                case SDLK_F2: return kc_f2;
                case SDLK_F3: return kc_f3;
                case SDLK_F4: return kc_f4;
                case SDLK_F5: return kc_f5;
                case SDLK_F6: return kc_f6;
                case SDLK_F7: return kc_f7;
                case SDLK_F8: return kc_f8;
                case SDLK_F9: return kc_f9;
                case SDLK_F10: return kc_f10;
                case SDLK_F11: return kc_f11;
                case SDLK_F12: return kc_f12;

                case SDLK_PRINTSCREEN: return kc_print_screen;
                case SDLK_SCROLLLOCK: return kc_scroll_lock;
                case SDLK_PAUSE: return kc_pause;
                case SDLK_INSERT: return kc_insert;
                case SDLK_HOME: return kc_home;
                case SDLK_PAGEUP: return kc_pageup;
                case SDLK_DELETE: return kc_delete;
                case SDLK_END: return kc_end;
                case SDLK_PAGEDOWN: return kc_pagedown;
                case SDLK_RIGHT: return kc_right;
                case SDLK_LEFT: return kc_left;
                case SDLK_DOWN: return kc_down;
                case SDLK_UP: return kc_up;

                case SDLK_NUMLOCKCLEAR: return kc_numlock_clear;
                case SDLK_KP_DIVIDE: return kc_kp_divide;
                case SDLK_KP_MULTIPLY: return kc_kp_multiply;
                case SDLK_KP_MINUS: return kc_kp_minus;
                case SDLK_KP_PLUS: return kc_kp_plus;
                case SDLK_KP_ENTER: return kc_kp_enter;
                case SDLK_KP_1: return kc_kp_1;
                case SDLK_KP_2: return kc_kp_2;
                case SDLK_KP_3: return kc_kp_3;
                case SDLK_KP_4: return kc_kp_4;
                case SDLK_KP_5: return kc_kp_5;
                case SDLK_KP_6: return kc_kp_6;
                case SDLK_KP_7: return kc_kp_7;
                case SDLK_KP_8: return kc_kp_8;
                case SDLK_KP_9: return kc_kp_9;
                case SDLK_KP_0: return kc_kp_0;
                case SDLK_KP_PERIOD: return kc_kp_period;

                case SDLK_APPLICATION: return kc_application;
                case SDLK_POWER: return kc_power;
                case SDLK_KP_EQUALS: return kc_kp_equals;
                case SDLK_F13: return kc_f13;
                case SDLK_F14: return kc_f14;
                case SDLK_F15: return kc_f15;
                case SDLK_F16: return kc_f16;
                case SDLK_F17: return kc_f17;
                case SDLK_F18: return kc_f18;
                case SDLK_F19: return kc_f19;
                case SDLK_F20: return kc_f20;
                case SDLK_F21: return kc_f21;
                case SDLK_F22: return kc_f22;
                case SDLK_F23: return kc_f23;
                case SDLK_F24: return kc_f24;
                case SDLK_EXECUTE: return kc_execute;
                case SDLK_HELP: return kc_help;
                case SDLK_MENU: return kc_menu;
                case SDLK_SELECT: return kc_select;
                case SDLK_STOP: return kc_stop;
                case SDLK_AGAIN: return kc_again;
                case SDLK_UNDO: return kc_undo;
                case SDLK_CUT: return kc_cut;
                case SDLK_COPY: return kc_copy;
                case SDLK_PASTE: return kc_paste;
                case SDLK_FIND: return kc_find;
                case SDLK_MUTE: return kc_mute;
                case SDLK_VOLUMEUP: return kc_volume_up;
                case SDLK_VOLUMEDOWN: return kc_volume_down;
                case SDLK_KP_COMMA: return kc_kp_comma;
                case SDLK_KP_EQUALSAS400: return kc_kp_equalsas400;

                case SDLK_ALTERASE: return kc_alterase;
                case SDLK_SYSREQ: return kc_sysreq;
                case SDLK_CANCEL: return kc_cancel;
                case SDLK_CLEAR: return kc_clear;
                case SDLK_PRIOR: return kc_prior;
                case SDLK_RETURN2: return kc_return2;
                case SDLK_SEPARATOR: return kc_separator;
                case SDLK_OUT: return kc_out;
                case SDLK_OPER: return kc_oper;
                case SDLK_CLEARAGAIN: return kc_clear_again;
                case SDLK_CRSEL: return kc_crsel;
                case SDLK_EXSEL: return kc_exsel;

                case SDLK_KP_00: return kc_kp_00;
                case SDLK_KP_000: return kc_kp_000;
                case SDLK_THOUSANDSSEPARATOR: return kc_thousands_separator;
                case SDLK_DECIMALSEPARATOR: return kc_decimal_separator;
                case SDLK_CURRENCYUNIT: return kc_currency_unit;
                case SDLK_CURRENCYSUBUNIT: return kc_currency_subunit;
                case SDLK_KP_LEFTPAREN: return kc_kp_left_paren;
                case SDLK_KP_RIGHTPAREN: return kc_kp_right_paren;
                case SDLK_KP_LEFTBRACE: return kc_kp_left_brace;
                case SDLK_KP_RIGHTBRACE: return kc_kp_right_brace;
                case SDLK_KP_TAB: return kc_kp_tab;
                case SDLK_KP_BACKSPACE: return kc_kp_backspace;
                case SDLK_KP_A: return kc_kp_A;
                case SDLK_KP_B: return kc_kp_B;
                case SDLK_KP_C: return kc_kp_C;
                case SDLK_KP_D: return kc_kp_D;
                case SDLK_KP_E: return kc_kp_E;
                case SDLK_KP_F: return kc_kp_F;
                case SDLK_KP_XOR: return kc_kp_xor;
                case SDLK_KP_POWER: return kc_kp_power;
                case SDLK_KP_PERCENT: return kc_kp_percent;
                case SDLK_KP_LESS: return kc_kp_less;
                case SDLK_KP_GREATER: return kc_kp_greater;
                case SDLK_KP_AMPERSAND: return kc_kp_ampersand;
                case SDLK_KP_DBLAMPERSAND: return kc_kp_dbl_ampersand;
                case SDLK_KP_VERTICALBAR: return kc_kp_vertical_bar;
                case SDLK_KP_DBLVERTICALBAR: return kc_kp_dbl_vertical_bar;
                case SDLK_KP_COLON: return kc_kp_colon;
                case SDLK_KP_HASH: return kc_kp_hash;
                case SDLK_KP_SPACE: return kc_kp_space;
                case SDLK_KP_AT: return kc_kp_at;
                case SDLK_KP_EXCLAM: return kc_kp_exclam;
                case SDLK_KP_MEMSTORE: return kc_kp_mem_store;
                case SDLK_KP_MEMRECALL: return kc_kp_mem_recall;
                case SDLK_KP_MEMCLEAR: return kc_kp_mem_clear;
                case SDLK_KP_MEMADD: return kc_kp_mem_add;
                case SDLK_KP_MEMSUBTRACT: return kc_kp_mem_subtract;
                case SDLK_KP_MEMMULTIPLY: return kc_kp_mem_multiply;
                case SDLK_KP_MEMDIVIDE: return kc_kp_mem_divide;
                case SDLK_KP_PLUSMINUS: return kc_kp_plus_minus;
                case SDLK_KP_CLEAR: return kc_kp_clear;
                case SDLK_KP_CLEARENTRY: return kc_kp_clear_entry;
                case SDLK_KP_BINARY: return kc_kp_binary;
                case SDLK_KP_OCTAL: return kc_kp_octal;
                case SDLK_KP_DECIMAL: return kc_kp_decimal;
                case SDLK_KP_HEXADECIMAL: return kc_kp_hexadecimal;

                case SDLK_LCTRL: return kc_lctrl;
                case SDLK_LSHIFT: return kc_lshift;
                case SDLK_LALT: return kc_lalt;
                case SDLK_LGUI: return kc_lgui;
                case SDLK_RCTRL: return kc_rctrl;
                case SDLK_RSHIFT: return kc_rshift;
                case SDLK_RALT: return kc_ralt;
                case SDLK_RGUI: return kc_rgui;

                case SDLK_MODE: return kc_mode;

                case SDLK_AUDIONEXT: return kc_audio_next;
                case SDLK_AUDIOPREV: return kc_audio_prev;
                case SDLK_AUDIOSTOP: return kc_audio_stop;
                case SDLK_AUDIOPLAY: return kc_audio_play;
                case SDLK_AUDIOMUTE: return kc_audio_mute;
                case SDLK_MEDIASELECT: return kc_media_select;
                case SDLK_WWW: return kc_www;
                case SDLK_MAIL: return kc_mail;
                case SDLK_CALCULATOR: return kc_calculator;
                case SDLK_COMPUTER: return kc_computer;
                case SDLK_AC_SEARCH: return kc_ac_search;
                case SDLK_AC_HOME: return kc_ac_home;
                case SDLK_AC_BACK: return kc_ac_back;
                case SDLK_AC_FORWARD: return kc_ac_forward;
                case SDLK_AC_STOP: return kc_ac_stop;
                case SDLK_AC_REFRESH: return kc_ac_refresh;
                case SDLK_AC_BOOKMARKS: return kc_ac_bookmarks;

                case SDLK_BRIGHTNESSDOWN: return kc_brightness_down;
                case SDLK_BRIGHTNESSUP: return kc_brightness_up;
                case SDLK_DISPLAYSWITCH: return kc_display_switch;
                case SDLK_KBDILLUMTOGGLE: return kc_kbb_illum_toggle;
                case SDLK_KBDILLUMDOWN: return kc_kbd_illum_down;
                case SDLK_KBDILLUMUP: return kc_kbd_illum_up;
                case SDLK_EJECT: return kc_eject;
                case SDLK_SLEEP: return kc_sleep;
                case SDLK_APP1: return kc_app1;
                case SDLK_APP2: return kc_app2;

                case SDLK_AUDIOREWIND: return kc_audio_rewind;
                case SDLK_AUDIOFASTFORWARD: return kc_audio_fast_forward;

                default: return kc_unknown;
            }

        }

        static keycode from_mouse_button(int32_t _sdl) {
            switch (_sdl) {
                case SDL_BUTTON_LEFT: return kc_mouse_left;
                case SDL_BUTTON_MIDDLE: return kc_mouse_middle;
                case SDL_BUTTON_RIGHT: return kc_mouse_right;
                case SDL_BUTTON_X1: return kc_mouse_x1;
                case SDL_BUTTON_X2: return kc_mouse_x2;
                default: return kc_unknown;
            }
        }

        static keycode from_gamepad_button(SDL_GameControllerButton _sdl) {
            switch (_sdl) {
                case SDL_CONTROLLER_BUTTON_INVALID: return kc_gamepad_button_invalid;
                case SDL_CONTROLLER_BUTTON_A: return kc_gamepad_button_a;
                case SDL_CONTROLLER_BUTTON_B: return kc_gamepad_button_b;
                case SDL_CONTROLLER_BUTTON_X: return kc_gamepad_button_x;
                case SDL_CONTROLLER_BUTTON_Y: return kc_gamepad_button_y;
                case SDL_CONTROLLER_BUTTON_BACK: return kc_gamepad_button_back;
                case SDL_CONTROLLER_BUTTON_GUIDE: return kc_gamepad_button_guide;
                case SDL_CONTROLLER_BUTTON_START: return kc_gamepad_button_start;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK: return kc_gamepad_button_lstick;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return kc_gamepad_button_rstick;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return kc_gamepad_button_lshoulder;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return kc_gamepad_button_rshoulder;
                case SDL_CONTROLLER_BUTTON_DPAD_UP: return kc_gamepad_button_dpad_up;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return kc_gamepad_button_dpad_down;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return kc_gamepad_button_dpad_left;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return kc_gamepad_button_dpad_right;
                case SDL_CONTROLLER_BUTTON_MISC1: return kc_gamepad_button_misc1; // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button.
                case SDL_CONTROLLER_BUTTON_PADDLE1: return kc_gamepad_button_paddle1; // Xbox Elite paddle P1.
                case SDL_CONTROLLER_BUTTON_PADDLE2: return kc_gamepad_button_paddle2; // Xbox Elite paddle P3.
                case SDL_CONTROLLER_BUTTON_PADDLE3: return kc_gamepad_button_paddle3; // Xbox Elite paddle P2.
                case SDL_CONTROLLER_BUTTON_PADDLE4: return kc_gamepad_button_paddle4; // Xbox Elite paddle P4.
                case SDL_CONTROLLER_BUTTON_TOUCHPAD: return kc_gamepad_button_touchpad; // PS5 touchpad button.
                default: return kc_gamepad_button_invalid;
            }
        }

        static keycode from_gamepad_axis(SDL_GameControllerAxis _sdl) {
            switch (_sdl) {
                case SDL_CONTROLLER_AXIS_INVALID: return kc_gamepad_axis_invalid;
                case SDL_CONTROLLER_AXIS_LEFTX: return kc_gamepad_axis_left_x;
                case SDL_CONTROLLER_AXIS_LEFTY: return kc_gamepad_axis_left_y;
                case SDL_CONTROLLER_AXIS_RIGHTX: return kc_gamepad_axis_right_x;
                case SDL_CONTROLLER_AXIS_RIGHTY: return kc_gamepad_axis_right_y;
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return kc_gamepad_axis_trigger_left;
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return kc_gamepad_axis_trigger_right;
                default: return kc_gamepad_axis_invalid;
            }
        }
    };

} // mkr