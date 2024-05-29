#include <log/log.h>
#include "application/sdl_message_pump.h"
#include "input/input_helper.h"
#include "input/input_manager.h"
#include "input/sdl_to_keycode.h"
#include "input/sdl_to_controller_index.h"

namespace mkr {
    void input_manager::sdl_event_callback(const event* _event) {
        const auto& e = static_cast<const sdl_event*>(_event)->sdl_event_;
        switch (e.type) {
            // Keyboard
            case SDL_KEYDOWN: {
                // Keyboard defaults to controller_index_0.
                const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_keycode::from_keyboard_button((SDL_KeyCode) e.key.keysym.sym));
                button_handler_.on_key_down(mask);
                axis_handler_.on_key_down(mask);
            }
                break;
            case SDL_KEYUP: {
                // Keyboard defaults to controller_index_0.
                const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_keycode::from_keyboard_button((SDL_KeyCode) e.key.keysym.sym));
                button_handler_.on_key_up(mask);
                axis_handler_.on_key_up(mask);
            }
                break;

            // Mouse
            case SDL_MOUSEBUTTONDOWN: {
                // Mouse defaults to controller_index_0.
                const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_keycode::from_mouse_button(e.button.button));
                button_handler_.on_key_down(mask);
                click_handler_.on_key_down(mask);
            }
                break;
            case SDL_MOUSEBUTTONUP: {
                // Mouse defaults to controller_index_0.
                const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_keycode::from_mouse_button(e.button.button));
                button_handler_.on_key_up(mask);
                click_handler_.on_key_up(mask);
            }
                break;
            case SDL_MOUSEMOTION: {
                // Mouse defaults to controller_index_0.
                const vector2 pos{(float) e.motion.x, (float) e.motion.y};
                const vector2 delta{(float) e.motion.xrel, (float) e.motion.yrel};

                // Clicks
                if (e.motion.state & SDL_BUTTON_LMASK) {
                    click_handler_.on_motion(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_left), pos);
                }
                if (e.motion.state & SDL_BUTTON_RMASK) {
                    click_handler_.on_motion(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_right), pos);
                }
                if (e.motion.state & SDL_BUTTON_MMASK) {
                    click_handler_.on_motion(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_middle), pos);
                }
                if (e.motion.state & SDL_BUTTON_X1MASK) {
                    click_handler_.on_motion(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_x1), pos);
                }
                if (e.motion.state & SDL_BUTTON_X2MASK) {
                    click_handler_.on_motion(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_x2), pos);
                }

                // Axis
                axis_handler_.on_axis(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_axis_x), delta.x_);
                axis_handler_.on_axis(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_axis_y), delta.y_);

                // Motion
                motion_handler_.on_motion(input_helper::get_input_mask(input_context_, controller_index_0, kc_mouse_motion), pos, delta);
            }
                break;

            // Gamepad
            case SDL_CONTROLLERAXISMOTION: {
                // Normalise the axis value to between [-1, 1]. SDL gamepad axis values range is [-32768, 32767].
                const auto value = e.caxis.value < 0 ? (float) e.caxis.value / -32768.0f : (float) e.caxis.value / 32767.0f;
                const input_mask_t mask = input_helper::get_input_mask(input_context_,
                                                                       sdl_to_controller_index::from_joystick_id(e.caxis.which),
                                                                       sdl_to_keycode::from_gamepad_axis((SDL_GameControllerAxis) e.caxis.axis));
                axis_handler_.on_axis(mask, value);
            }
                break;
            case SDL_CONTROLLERBUTTONDOWN: {
                const input_mask_t mask = input_helper::get_input_mask(input_context_,
                                                                       sdl_to_controller_index::from_joystick_id(e.cbutton.which),
                                                                       sdl_to_keycode::from_gamepad_button((SDL_GameControllerButton) e.cbutton.button));
                button_handler_.on_key_down(mask);
            }
                break;
            case SDL_CONTROLLERBUTTONUP: {
                const input_mask_t mask = input_helper::get_input_mask(input_context_,
                                                                       sdl_to_controller_index::from_joystick_id(e.cbutton.which),
                                                                       sdl_to_keycode::from_gamepad_button((SDL_GameControllerButton) e.cbutton.button));
                button_handler_.on_key_up(mask);
            }
                break;
            default:
                break;
        }
    }

    void input_manager::init() {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
        if (0 != SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC)) {
            const std::string err_msg = "SDL_INIT_GAMECONTROLLER or SDL_INIT_JOYSTICK or SDL_INIT_HAPTIC failed";
            MKR_CORE_ERROR(err_msg);
            throw std::runtime_error(err_msg);
        }
        SDL_JoystickEventState(SDL_ENABLE);
        SDL_GameControllerEventState(SDL_ENABLE);

        sdl_event_listener.set_callback(std::bind(&input_manager::sdl_event_callback, this, std::placeholders::_1));
        sdl_message_pump::instance().get_event_dispatcher().add_listener<sdl_event>(&sdl_event_listener);
    }

    void input_manager::update() {
        button_handler_.dispatch_events(input_event_dispatcher_);
        axis_handler_.dispatch_events(input_event_dispatcher_);
        click_handler_.dispatch_events(input_event_dispatcher_);
        motion_handler_.dispatch_events(input_event_dispatcher_);
    }

    void input_manager::exit() {
        sdl_message_pump::instance().get_event_dispatcher().remove_listener<sdl_event>(&sdl_event_listener);
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
    }

    void input_manager::register_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _button) {
        button_handler_.register_button(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _button));
    }

    void input_manager::unregister_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _button) {
        button_handler_.unregister_button(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _button));
    }

    void input_manager::register_click(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _click) {
        click_handler_.register_click(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _click));
    }

    void input_manager::unregister_click(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _click) {
        click_handler_.unregister_click(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _click));
    }

    void input_manager::register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _axis) {
        axis_handler_.register_axis(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _axis));
    }

    void input_manager::unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _axis) {
        axis_handler_.unregister_axis(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _axis));
    }

    void input_manager::register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _positive_button, mkr::keycode _negative_button) {
        axis_handler_.register_button(_input_action,
                                      input_helper::get_input_mask(_input_context, _controller_index, _positive_button),
                                      input_helper::get_input_mask(_input_context, _controller_index, _negative_button));
    }

    void input_manager::unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _positive_button, mkr::keycode _negative_button) {
        axis_handler_.unregister_button(_input_action,
                                        input_helper::get_input_mask(_input_context, _controller_index, _positive_button),
                                        input_helper::get_input_mask(_input_context, _controller_index, _negative_button));
    }

    void input_manager::register_motion(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _motion) {
        motion_handler_.register_motion(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _motion));
    }

    void input_manager::unregister_motion(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _motion) {
        motion_handler_.unregister_motion(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _motion));
    }
} // mkr