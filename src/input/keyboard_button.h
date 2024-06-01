#pragma once

#include "input/input.h"

namespace mkr {
// Keyboard Button Range: [0, 300)
enum keyboard_button : keycode_t {
    keyboard_invalid = 0,

    keyboard_unknown,

    keyboard_return,
    keyboard_escape,
    keyboard_backspace,
    keyboard_tab,
    keyboard_space,
    keyboard_exclaim,
    keyboard_quote_dbl,
    keyboard_hash,
    keyboard_percent,
    keyboard_dollar,
    keyboard_ampersand,
    keyboard_quote,
    keyboard_left_paren,
    keyboard_right_paren,
    keyboard_asterisk,
    keyboard_plus,
    keyboard_comma,
    keyboard_minus,
    keyboard_period,
    keyboard_slash,
    keyboard_0,
    keyboard_1,
    keyboard_2,
    keyboard_3,
    keyboard_4,
    keyboard_5,
    keyboard_6,
    keyboard_7,
    keyboard_8,
    keyboard_9,
    keyboard_colon,
    keyboard_semicolon,
    keyboard_less,
    keyboard_equals,
    keyboard_greater,
    keyboard_question,
    keyboard_at,

    keyboard_left_bracket,
    keyboard_backslash,
    keyboard_right_bracket,
    keyboard_caret,
    keyboard_underscore,
    keyboard_backquote,
    keyboard_a,
    keyboard_b,
    keyboard_c,
    keyboard_d,
    keyboard_e,
    keyboard_f,
    keyboard_g,
    keyboard_h,
    keyboard_i,
    keyboard_j,
    keyboard_k,
    keyboard_l,
    keyboard_m,
    keyboard_n,
    keyboard_o,
    keyboard_p,
    keyboard_q,
    keyboard_r,
    keyboard_s,
    keyboard_t,
    keyboard_u,
    keyboard_v,
    keyboard_w,
    keyboard_x,
    keyboard_y,
    keyboard_z,

    keyboard_capslock,

    keyboard_f1,
    keyboard_f2,
    keyboard_f3,
    keyboard_f4,
    keyboard_f5,
    keyboard_f6,
    keyboard_f7,
    keyboard_f8,
    keyboard_f9,
    keyboard_f10,
    keyboard_f11,
    keyboard_f12,

    keyboard_print_screen,
    keyboard_scroll_lock,
    keyboard_pause,
    keyboard_insert,
    keyboard_home,
    keyboard_pageup,
    keyboard_delete,
    keyboard_end,
    keyboard_pagedown,
    keyboard_right,
    keyboard_left,
    keyboard_down,
    keyboard_up,

    keyboard_numlock_clear,
    keyboard_kp_divide,
    keyboard_kp_multiply,
    keyboard_kp_minus,
    keyboard_kp_plus,
    keyboard_kp_enter,
    keyboard_kp_1,
    keyboard_kp_2,
    keyboard_kp_3,
    keyboard_kp_4,
    keyboard_kp_5,
    keyboard_kp_6,
    keyboard_kp_7,
    keyboard_kp_8,
    keyboard_kp_9,
    keyboard_kp_0,
    keyboard_kp_period,

    keyboard_application,
    keyboard_power,
    keyboard_kp_equals,
    keyboard_f13,
    keyboard_f14,
    keyboard_f15,
    keyboard_f16,
    keyboard_f17,
    keyboard_f18,
    keyboard_f19,
    keyboard_f20,
    keyboard_f21,
    keyboard_f22,
    keyboard_f23,
    keyboard_f24,
    keyboard_execute,
    keyboard_help,
    keyboard_menu,
    keyboard_select,
    keyboard_stop,
    keyboard_again,
    keyboard_undo,
    keyboard_cut,
    keyboard_copy,
    keyboard_paste,
    keyboard_find,
    keyboard_mute,
    keyboard_volume_up,
    keyboard_volume_down,
    keyboard_kp_comma,
    keyboard_kp_equalsas400,

    keyboard_alterase,
    keyboard_sysreq,
    keyboard_cancel,
    keyboard_clear,
    keyboard_prior,
    keyboard_return2,
    keyboard_separator,
    keyboard_out,
    keyboard_oper,
    keyboard_clear_again,
    keyboard_crsel,
    keyboard_exsel,

    keyboard_kp_00,
    keyboard_kp_000,
    keyboard_thousands_separator,
    keyboard_decimal_separator,
    keyboard_currency_unit,
    keyboard_currency_subunit,
    keyboard_kp_left_paren,
    keyboard_kp_right_paren,
    keyboard_kp_left_brace,
    keyboard_kp_right_brace,
    keyboard_kp_tab,
    keyboard_kp_backspace,
    keyboard_kp_A,
    keyboard_kp_B,
    keyboard_kp_C,
    keyboard_kp_D,
    keyboard_kp_E,
    keyboard_kp_F,
    keyboard_kp_xor,
    keyboard_kp_power,
    keyboard_kp_percent,
    keyboard_kp_less,
    keyboard_kp_greater,
    keyboard_kp_ampersand,
    keyboard_kp_dbl_ampersand,
    keyboard_kp_vertical_bar,
    keyboard_kp_dbl_vertical_bar,
    keyboard_kp_colon,
    keyboard_kp_hash,
    keyboard_kp_space,
    keyboard_kp_at,
    keyboard_kp_exclam,
    keyboard_kp_mem_store,
    keyboard_kp_mem_recall,
    keyboard_kp_mem_clear,
    keyboard_kp_mem_add,
    keyboard_kp_mem_subtract,
    keyboard_kp_mem_multiply,
    keyboard_kp_mem_divide,
    keyboard_kp_plus_minus,
    keyboard_kp_clear,
    keyboard_kp_clear_entry,
    keyboard_kp_binary,
    keyboard_kp_octal,
    keyboard_kp_decimal,
    keyboard_kp_hexadecimal,

    keyboard_lctrl,
    keyboard_lshift,
    keyboard_lalt,
    keyboard_lgui,
    keyboard_rctrl,
    keyboard_rshift,
    keyboard_ralt,
    keyboard_rgui,

    keyboard_mode,

    keyboard_audio_next,
    keyboard_audio_prev,
    keyboard_audio_stop,
    keyboard_audio_play,
    keyboard_audio_mute,
    keyboard_media_select,
    keyboard_www,
    keyboard_mail,
    keyboard_calculator,
    keyboard_computer,
    keyboard_ac_search,
    keyboard_ac_home,
    keyboard_ac_back,
    keyboard_ac_forward,
    keyboard_ac_stop,
    keyboard_ac_refresh,
    keyboard_ac_bookmarks,

    keyboard_brightness_down,
    keyboard_brightness_up,
    keyboard_display_switch,
    keyboard_kbb_illum_toggle,
    keyboard_kbd_illum_down,
    keyboard_kbd_illum_up,
    keyboard_eject,
    keyboard_sleep,
    keyboard_app1,
    keyboard_app2,

    keyboard_audio_rewind,
    keyboard_audio_fast_forward,

    keyboard_soft_left,
    keyboard_soft_right,
    keyboard_call,
    keyboard_end_call,
};
} // mkr