#pragma once

#include <cstdint>

namespace mkr {
    enum keycode : uint32_t {
        /********************************** Buttons (For Button Handler) **********************************/
        // Keyboard Buttons
        kc_unknown,
        kc_return,
        kc_escape,
        kc_backspace,
        kc_tab,
        kc_space,
        kc_exclaim,
        kc_quote_dbl,
        kc_hash,
        kc_percent,
        kc_dollar,
        kc_ampersand,
        kc_quote,
        kc_left_paren,
        kc_right_paren,
        kc_asterisk,
        kc_plus,
        kc_comma,
        kc_minus,
        kc_period,
        kc_slash,
        kc_0,
        kc_1,
        kc_2,
        kc_3,
        kc_4,
        kc_5,
        kc_6,
        kc_7,
        kc_8,
        kc_9,
        kc_colon,
        kc_semicolon,
        kc_less,
        kc_equals,
        kc_greater,
        kc_question,
        kc_at,

        kc_left_bracket,
        kc_backslash,
        kc_right_bracket,
        kc_caret,
        kc_underscore,
        kc_backquote,
        kc_a,
        kc_b,
        kc_c,
        kc_d,
        kc_e,
        kc_f,
        kc_g,
        kc_h,
        kc_i,
        kc_j,
        kc_k,
        kc_l,
        kc_m,
        kc_n,
        kc_o,
        kc_p,
        kc_q,
        kc_r,
        kc_s,
        kc_t,
        kc_u,
        kc_v,
        kc_w,
        kc_x,
        kc_y,
        kc_z,

        kc_capslock,

        kc_f1,
        kc_f2,
        kc_f3,
        kc_f4,
        kc_f5,
        kc_f6,
        kc_f7,
        kc_f8,
        kc_f9,
        kc_f10,
        kc_f11,
        kc_f12,

        kc_print_screen,
        kc_scroll_lock,
        kc_pause,
        kc_insert,
        kc_home,
        kc_pageup,
        kc_delete,
        kc_end,
        kc_pagedown,
        kc_right,
        kc_left,
        kc_down,
        kc_up,

        kc_numlock_clear,
        kc_kp_divide,
        kc_kp_multiply,
        kc_kp_minus,
        kc_kp_plus,
        kc_kp_enter,
        kc_kp_1,
        kc_kp_2,
        kc_kp_3,
        kc_kp_4,
        kc_kp_5,
        kc_kp_6,
        kc_kp_7,
        kc_kp_8,
        kc_kp_9,
        kc_kp_0,
        kc_kp_period,

        kc_application,
        kc_power,
        kc_kp_equals,
        kc_f13,
        kc_f14,
        kc_f15,
        kc_f16,
        kc_f17,
        kc_f18,
        kc_f19,
        kc_f20,
        kc_f21,
        kc_f22,
        kc_f23,
        kc_f24,
        kc_execute,
        kc_help,
        kc_menu,
        kc_select,
        kc_stop,
        kc_again,
        kc_undo,
        kc_cut,
        kc_copy,
        kc_paste,
        kc_find,
        kc_mute,
        kc_volume_up,
        kc_volume_down,
        kc_kp_comma,
        kc_kp_equalsas400,

        kc_alterase,
        kc_sysreq,
        kc_cancel,
        kc_clear,
        kc_prior,
        kc_return2,
        kc_separator,
        kc_out,
        kc_oper,
        kc_clear_again,
        kc_crsel,
        kc_exsel,

        kc_kp_00,
        kc_kp_000,
        kc_thousands_separator,
        kc_decimal_separator,
        kc_currency_unit,
        kc_currency_subunit,
        kc_kp_left_paren,
        kc_kp_right_paren,
        kc_kp_left_brace,
        kc_kp_right_brace,
        kc_kp_tab,
        kc_kp_backspace,
        kc_kp_A,
        kc_kp_B,
        kc_kp_C,
        kc_kp_D,
        kc_kp_E,
        kc_kp_F,
        kc_kp_xor,
        kc_kp_power,
        kc_kp_percent,
        kc_kp_less,
        kc_kp_greater,
        kc_kp_ampersand,
        kc_kp_dbl_ampersand,
        kc_kp_vertical_bar,
        kc_kp_dbl_vertical_bar,
        kc_kp_colon,
        kc_kp_hash,
        kc_kp_space,
        kc_kp_at,
        kc_kp_exclam,
        kc_kp_mem_store,
        kc_kp_mem_recall,
        kc_kp_mem_clear,
        kc_kp_mem_add,
        kc_kp_mem_subtract,
        kc_kp_mem_multiply,
        kc_kp_mem_divide,
        kc_kp_plus_minus,
        kc_kp_clear,
        kc_kp_clear_entry,
        kc_kp_binary,
        kc_kp_octal,
        kc_kp_decimal,
        kc_kp_hexadecimal,

        kc_lctrl,
        kc_lshift,
        kc_lalt,
        kc_lgui,
        kc_rctrl,
        kc_rshift,
        kc_ralt,
        kc_rgui,

        kc_mode,

        kc_audio_next,
        kc_audio_prev,
        kc_audio_stop,
        kc_audio_play,
        kc_audio_mute,
        kc_media_select,
        kc_www,
        kc_mail,
        kc_calculator,
        kc_computer,
        kc_ac_search,
        kc_ac_home,
        kc_ac_back,
        kc_ac_forward,
        kc_ac_stop,
        kc_ac_refresh,
        kc_ac_bookmarks,

        kc_brightness_down,
        kc_brightness_up,
        kc_display_switch,
        kc_kbb_illum_toggle,
        kc_kbd_illum_down,
        kc_kbd_illum_up,
        kc_eject,
        kc_sleep,
        kc_app1,
        kc_app2,

        kc_audio_rewind,
        kc_audio_fast_forward,

        // Gamepad Buttons
        kc_gamepad_button_invalid,
        kc_gamepad_button_a,
        kc_gamepad_button_b,
        kc_gamepad_button_x,
        kc_gamepad_button_y,
        kc_gamepad_button_back,
        kc_gamepad_button_guide,
        kc_gamepad_button_start,
        kc_gamepad_button_lstick,
        kc_gamepad_button_rstick,
        kc_gamepad_button_lshoulder,
        kc_gamepad_button_rshoulder,
        kc_gamepad_button_dpad_up,
        kc_gamepad_button_dpad_down,
        kc_gamepad_button_dpad_left,
        kc_gamepad_button_dpad_right,
        kc_gamepad_button_misc1,
        kc_gamepad_button_paddle1,
        kc_gamepad_button_paddle2,
        kc_gamepad_button_paddle3,
        kc_gamepad_button_paddle4,
        kc_gamepad_button_touchpad,

        /********************************** Button/Clicks (For Button Handler/Click Handler) **********************************/
        // Mouse Buttons
        kc_mouse_left,
        kc_mouse_right,
        kc_mouse_middle,
        kc_mouse_x1,
        kc_mouse_x2,

        /********************************** Axes (For Axes Handler) **********************************/
        // Mouse Axes
        kc_mouse_axis_x,
        kc_mouse_axis_y,

        // Gamepad Axes
        kc_gamepad_axis_invalid,
        kc_gamepad_axis_left_x,
        kc_gamepad_axis_left_y,
        kc_gamepad_axis_right_x,
        kc_gamepad_axis_right_y,
        kc_gamepad_axis_trigger_left,
        kc_gamepad_axis_trigger_right,

        /********************************** Motion (For Motion Handler) **********************************/
        // Mouse Motion
        kc_mouse_motion,
    };
} // mkr