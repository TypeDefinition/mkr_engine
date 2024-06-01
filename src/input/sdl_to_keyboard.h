#pragma once

#include <SDL2/SDL_keycode.h>
#include "input/keyboard_button.h"

namespace mkr {
class sdl_to_keyboard {
public:
    sdl_to_keyboard() = delete;

    static keyboard_button to_button(SDL_KeyCode _sdl) {
        switch (_sdl) {
        case SDLK_UNKNOWN: return keyboard_unknown;

        case SDLK_RETURN: return keyboard_return;
        case SDLK_ESCAPE: return keyboard_escape;
        case SDLK_BACKSPACE: return keyboard_backspace;
        case SDLK_TAB: return keyboard_tab;
        case SDLK_SPACE: return keyboard_space;
        case SDLK_EXCLAIM: return keyboard_exclaim;
        case SDLK_QUOTEDBL: return keyboard_quote_dbl;
        case SDLK_HASH: return keyboard_hash;
        case SDLK_PERCENT: return keyboard_percent;
        case SDLK_DOLLAR: return keyboard_dollar;
        case SDLK_AMPERSAND: return keyboard_ampersand;
        case SDLK_QUOTE: return keyboard_quote;
        case SDLK_LEFTPAREN: return keyboard_left_paren;
        case SDLK_RIGHTPAREN: return keyboard_right_paren;
        case SDLK_ASTERISK: return keyboard_asterisk;
        case SDLK_PLUS: return keyboard_plus;
        case SDLK_COMMA: return keyboard_comma;
        case SDLK_MINUS: return keyboard_minus;
        case SDLK_PERIOD: return keyboard_period;
        case SDLK_SLASH: return keyboard_slash;
        case SDLK_0: return keyboard_0;
        case SDLK_1: return keyboard_1;
        case SDLK_2: return keyboard_2;
        case SDLK_3: return keyboard_3;
        case SDLK_4: return keyboard_4;
        case SDLK_5: return keyboard_5;
        case SDLK_6: return keyboard_6;
        case SDLK_7: return keyboard_7;
        case SDLK_8: return keyboard_8;
        case SDLK_9: return keyboard_9;
        case SDLK_COLON: return keyboard_colon;
        case SDLK_SEMICOLON: return keyboard_semicolon;
        case SDLK_LESS: return keyboard_less;
        case SDLK_EQUALS: return keyboard_equals;
        case SDLK_GREATER: return keyboard_greater;
        case SDLK_QUESTION: return keyboard_question;
        case SDLK_AT: return keyboard_at;

        case SDLK_LEFTBRACKET: return keyboard_left_bracket;
        case SDLK_BACKSLASH: return keyboard_backslash;
        case SDLK_RIGHTBRACKET: return keyboard_right_bracket;
        case SDLK_CARET: return keyboard_caret;
        case SDLK_UNDERSCORE: return keyboard_underscore;
        case SDLK_BACKQUOTE: return keyboard_backquote;
        case SDLK_a: return keyboard_a;
        case SDLK_b: return keyboard_b;
        case SDLK_c: return keyboard_c;
        case SDLK_d: return keyboard_d;
        case SDLK_e: return keyboard_e;
        case SDLK_f: return keyboard_f;
        case SDLK_g: return keyboard_g;
        case SDLK_h: return keyboard_h;
        case SDLK_i: return keyboard_i;
        case SDLK_j: return keyboard_j;
        case SDLK_k: return keyboard_k;
        case SDLK_l: return keyboard_l;
        case SDLK_m: return keyboard_m;
        case SDLK_n: return keyboard_n;
        case SDLK_o: return keyboard_o;
        case SDLK_p: return keyboard_p;
        case SDLK_q: return keyboard_q;
        case SDLK_r: return keyboard_r;
        case SDLK_s: return keyboard_s;
        case SDLK_t: return keyboard_t;
        case SDLK_u: return keyboard_u;
        case SDLK_v: return keyboard_v;
        case SDLK_w: return keyboard_w;
        case SDLK_x: return keyboard_x;
        case SDLK_y: return keyboard_y;
        case SDLK_z: return keyboard_z;

        case SDLK_CAPSLOCK: return keyboard_capslock;

        case SDLK_F1: return keyboard_f1;
        case SDLK_F2: return keyboard_f2;
        case SDLK_F3: return keyboard_f3;
        case SDLK_F4: return keyboard_f4;
        case SDLK_F5: return keyboard_f5;
        case SDLK_F6: return keyboard_f6;
        case SDLK_F7: return keyboard_f7;
        case SDLK_F8: return keyboard_f8;
        case SDLK_F9: return keyboard_f9;
        case SDLK_F10: return keyboard_f10;
        case SDLK_F11: return keyboard_f11;
        case SDLK_F12: return keyboard_f12;

        case SDLK_PRINTSCREEN: return keyboard_print_screen;
        case SDLK_SCROLLLOCK: return keyboard_scroll_lock;
        case SDLK_PAUSE: return keyboard_pause;
        case SDLK_INSERT: return keyboard_insert;
        case SDLK_HOME: return keyboard_home;
        case SDLK_PAGEUP: return keyboard_pageup;
        case SDLK_DELETE: return keyboard_delete;
        case SDLK_END: return keyboard_end;
        case SDLK_PAGEDOWN: return keyboard_pagedown;
        case SDLK_RIGHT: return keyboard_right;
        case SDLK_LEFT: return keyboard_left;
        case SDLK_DOWN: return keyboard_down;
        case SDLK_UP: return keyboard_up;

        case SDLK_NUMLOCKCLEAR: return keyboard_numlock_clear;
        case SDLK_KP_DIVIDE: return keyboard_kp_divide;
        case SDLK_KP_MULTIPLY: return keyboard_kp_multiply;
        case SDLK_KP_MINUS: return keyboard_kp_minus;
        case SDLK_KP_PLUS: return keyboard_kp_plus;
        case SDLK_KP_ENTER: return keyboard_kp_enter;
        case SDLK_KP_1: return keyboard_kp_1;
        case SDLK_KP_2: return keyboard_kp_2;
        case SDLK_KP_3: return keyboard_kp_3;
        case SDLK_KP_4: return keyboard_kp_4;
        case SDLK_KP_5: return keyboard_kp_5;
        case SDLK_KP_6: return keyboard_kp_6;
        case SDLK_KP_7: return keyboard_kp_7;
        case SDLK_KP_8: return keyboard_kp_8;
        case SDLK_KP_9: return keyboard_kp_9;
        case SDLK_KP_0: return keyboard_kp_0;
        case SDLK_KP_PERIOD: return keyboard_kp_period;

        case SDLK_APPLICATION: return keyboard_application;
        case SDLK_POWER: return keyboard_power;
        case SDLK_KP_EQUALS: return keyboard_kp_equals;
        case SDLK_F13: return keyboard_f13;
        case SDLK_F14: return keyboard_f14;
        case SDLK_F15: return keyboard_f15;
        case SDLK_F16: return keyboard_f16;
        case SDLK_F17: return keyboard_f17;
        case SDLK_F18: return keyboard_f18;
        case SDLK_F19: return keyboard_f19;
        case SDLK_F20: return keyboard_f20;
        case SDLK_F21: return keyboard_f21;
        case SDLK_F22: return keyboard_f22;
        case SDLK_F23: return keyboard_f23;
        case SDLK_F24: return keyboard_f24;
        case SDLK_EXECUTE: return keyboard_execute;
        case SDLK_HELP: return keyboard_help;
        case SDLK_MENU: return keyboard_menu;
        case SDLK_SELECT: return keyboard_select;
        case SDLK_STOP: return keyboard_stop;
        case SDLK_AGAIN: return keyboard_again;
        case SDLK_UNDO: return keyboard_undo;
        case SDLK_CUT: return keyboard_cut;
        case SDLK_COPY: return keyboard_copy;
        case SDLK_PASTE: return keyboard_paste;
        case SDLK_FIND: return keyboard_find;
        case SDLK_MUTE: return keyboard_mute;
        case SDLK_VOLUMEUP: return keyboard_volume_up;
        case SDLK_VOLUMEDOWN: return keyboard_volume_down;
        case SDLK_KP_COMMA: return keyboard_kp_comma;
        case SDLK_KP_EQUALSAS400: return keyboard_kp_equalsas400;

        case SDLK_ALTERASE: return keyboard_alterase;
        case SDLK_SYSREQ: return keyboard_sysreq;
        case SDLK_CANCEL: return keyboard_cancel;
        case SDLK_CLEAR: return keyboard_clear;
        case SDLK_PRIOR: return keyboard_prior;
        case SDLK_RETURN2: return keyboard_return2;
        case SDLK_SEPARATOR: return keyboard_separator;
        case SDLK_OUT: return keyboard_out;
        case SDLK_OPER: return keyboard_oper;
        case SDLK_CLEARAGAIN: return keyboard_clear_again;
        case SDLK_CRSEL: return keyboard_crsel;
        case SDLK_EXSEL: return keyboard_exsel;

        case SDLK_KP_00: return keyboard_kp_00;
        case SDLK_KP_000: return keyboard_kp_000;
        case SDLK_THOUSANDSSEPARATOR: return keyboard_thousands_separator;
        case SDLK_DECIMALSEPARATOR: return keyboard_decimal_separator;
        case SDLK_CURRENCYUNIT: return keyboard_currency_unit;
        case SDLK_CURRENCYSUBUNIT: return keyboard_currency_subunit;
        case SDLK_KP_LEFTPAREN: return keyboard_kp_left_paren;
        case SDLK_KP_RIGHTPAREN: return keyboard_kp_right_paren;
        case SDLK_KP_LEFTBRACE: return keyboard_kp_left_brace;
        case SDLK_KP_RIGHTBRACE: return keyboard_kp_right_brace;
        case SDLK_KP_TAB: return keyboard_kp_tab;
        case SDLK_KP_BACKSPACE: return keyboard_kp_backspace;
        case SDLK_KP_A: return keyboard_kp_A;
        case SDLK_KP_B: return keyboard_kp_B;
        case SDLK_KP_C: return keyboard_kp_C;
        case SDLK_KP_D: return keyboard_kp_D;
        case SDLK_KP_E: return keyboard_kp_E;
        case SDLK_KP_F: return keyboard_kp_F;
        case SDLK_KP_XOR: return keyboard_kp_xor;
        case SDLK_KP_POWER: return keyboard_kp_power;
        case SDLK_KP_PERCENT: return keyboard_kp_percent;
        case SDLK_KP_LESS: return keyboard_kp_less;
        case SDLK_KP_GREATER: return keyboard_kp_greater;
        case SDLK_KP_AMPERSAND: return keyboard_kp_ampersand;
        case SDLK_KP_DBLAMPERSAND: return keyboard_kp_dbl_ampersand;
        case SDLK_KP_VERTICALBAR: return keyboard_kp_vertical_bar;
        case SDLK_KP_DBLVERTICALBAR: return keyboard_kp_dbl_vertical_bar;
        case SDLK_KP_COLON: return keyboard_kp_colon;
        case SDLK_KP_HASH: return keyboard_kp_hash;
        case SDLK_KP_SPACE: return keyboard_kp_space;
        case SDLK_KP_AT: return keyboard_kp_at;
        case SDLK_KP_EXCLAM: return keyboard_kp_exclam;
        case SDLK_KP_MEMSTORE: return keyboard_kp_mem_store;
        case SDLK_KP_MEMRECALL: return keyboard_kp_mem_recall;
        case SDLK_KP_MEMCLEAR: return keyboard_kp_mem_clear;
        case SDLK_KP_MEMADD: return keyboard_kp_mem_add;
        case SDLK_KP_MEMSUBTRACT: return keyboard_kp_mem_subtract;
        case SDLK_KP_MEMMULTIPLY: return keyboard_kp_mem_multiply;
        case SDLK_KP_MEMDIVIDE: return keyboard_kp_mem_divide;
        case SDLK_KP_PLUSMINUS: return keyboard_kp_plus_minus;
        case SDLK_KP_CLEAR: return keyboard_kp_clear;
        case SDLK_KP_CLEARENTRY: return keyboard_kp_clear_entry;
        case SDLK_KP_BINARY: return keyboard_kp_binary;
        case SDLK_KP_OCTAL: return keyboard_kp_octal;
        case SDLK_KP_DECIMAL: return keyboard_kp_decimal;
        case SDLK_KP_HEXADECIMAL: return keyboard_kp_hexadecimal;

        case SDLK_LCTRL: return keyboard_lctrl;
        case SDLK_LSHIFT: return keyboard_lshift;
        case SDLK_LALT: return keyboard_lalt;
        case SDLK_LGUI: return keyboard_lgui;
        case SDLK_RCTRL: return keyboard_rctrl;
        case SDLK_RSHIFT: return keyboard_rshift;
        case SDLK_RALT: return keyboard_ralt;
        case SDLK_RGUI: return keyboard_rgui;

        case SDLK_MODE: return keyboard_mode;

        case SDLK_AUDIONEXT: return keyboard_audio_next;
        case SDLK_AUDIOPREV: return keyboard_audio_prev;
        case SDLK_AUDIOSTOP: return keyboard_audio_stop;
        case SDLK_AUDIOPLAY: return keyboard_audio_play;
        case SDLK_AUDIOMUTE: return keyboard_audio_mute;
        case SDLK_MEDIASELECT: return keyboard_media_select;
        case SDLK_WWW: return keyboard_www;
        case SDLK_MAIL: return keyboard_mail;
        case SDLK_CALCULATOR: return keyboard_calculator;
        case SDLK_COMPUTER: return keyboard_computer;
        case SDLK_AC_SEARCH: return keyboard_ac_search;
        case SDLK_AC_HOME: return keyboard_ac_home;
        case SDLK_AC_BACK: return keyboard_ac_back;
        case SDLK_AC_FORWARD: return keyboard_ac_forward;
        case SDLK_AC_STOP: return keyboard_ac_stop;
        case SDLK_AC_REFRESH: return keyboard_ac_refresh;
        case SDLK_AC_BOOKMARKS: return keyboard_ac_bookmarks;

        case SDLK_BRIGHTNESSDOWN: return keyboard_brightness_down;
        case SDLK_BRIGHTNESSUP: return keyboard_brightness_up;
        case SDLK_DISPLAYSWITCH: return keyboard_display_switch;
        case SDLK_KBDILLUMTOGGLE: return keyboard_kbb_illum_toggle;
        case SDLK_KBDILLUMDOWN: return keyboard_kbd_illum_down;
        case SDLK_KBDILLUMUP: return keyboard_kbd_illum_up;
        case SDLK_EJECT: return keyboard_eject;
        case SDLK_SLEEP: return keyboard_sleep;
        case SDLK_APP1: return keyboard_app1;
        case SDLK_APP2: return keyboard_app2;

        case SDLK_AUDIOREWIND: return keyboard_audio_rewind;
        case SDLK_AUDIOFASTFORWARD: return keyboard_audio_fast_forward;

        case SDLK_SOFTLEFT: return keyboard_soft_left;
        case SDLK_SOFTRIGHT: return keyboard_soft_right;
        case SDLK_CALL: return keyboard_call;
        case SDLK_ENDCALL: return keyboard_end_call;
        }
        return keyboard_invalid;
    }
};
}
