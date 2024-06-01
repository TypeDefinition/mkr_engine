#include <log/log.h>
#include "input/input_helper.h"
#include "input/input_manager.h"
#include "input/sdl_to_keyboard.h"
#include "input/sdl_to_mouse.h"
#include "input/sdl_to_gamepad.h"

namespace mkr {
    void input_manager::on_sdl_event(const sdl_event* _event) {
        const auto& e = _event->sdl_event_;
        switch (e.type) {
        // Keyboard
        case SDL_KEYDOWN: {
            // Keyboard defaults to controller_index_0.
            const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_keyboard::to_button((SDL_KeyCode) e.key.keysym.sym));
            button_handler_.on_button_down(mask);
            axis_handler_.on_button_down(mask);
        }
        break;
        case SDL_KEYUP: {
            // Keyboard defaults to controller_index_0.
            const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_keyboard::to_button((SDL_KeyCode) e.key.keysym.sym));
            button_handler_.on_button_up(mask);
            axis_handler_.on_button_up(mask);
        }
        break;

        // Mouse
        case SDL_MOUSEBUTTONDOWN: {
            // Mouse defaults to controller_index_0.
            const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_mouse::to_button(e.button.button));
            button_handler_.on_button_down(mask);
            axis_handler_.on_button_down(mask);
        }
        break;
        case SDL_MOUSEBUTTONUP: {
            // Mouse defaults to controller_index_0.
            const input_mask_t mask = input_helper::get_input_mask(input_context_, controller_index_0, sdl_to_mouse::to_button(e.button.button));
            button_handler_.on_button_up(mask);
            axis_handler_.on_button_up(mask);
        }
        break;
        case SDL_MOUSEMOTION: {
            // Mouse defaults to controller_index_0.
            axis_handler_.on_axis(input_helper::get_input_mask(input_context_, controller_index_0, mouse_axis_x), (float) e.motion.xrel);
            axis_handler_.on_axis(input_helper::get_input_mask(input_context_, controller_index_0, mouse_axis_y), (float) e.motion.yrel);
        }
        break;

        // Gamepad
        case SDL_CONTROLLERAXISMOTION: {
            // Normalise the axis value to between [-1, 1]. SDL gamepad axis values range is [-32768, 32767].
            const auto value = e.caxis.value < 0 ? (float) e.caxis.value / -32768.0f : (float) e.caxis.value / 32767.0f;
            const input_mask_t mask = input_helper::get_input_mask(input_context_,
                                                                   sdl_to_gamepad::to_controller_index(e.caxis.which),
                                                                   sdl_to_gamepad::to_axis((SDL_GameControllerAxis) e.caxis.axis));
            axis_handler_.on_axis(mask, value);
        }
        break;
        case SDL_CONTROLLERBUTTONDOWN: {
            const input_mask_t mask = input_helper::get_input_mask(input_context_,
                                                                   sdl_to_gamepad::to_controller_index(e.cbutton.which),
                                                                   sdl_to_gamepad::to_button((SDL_GameControllerButton) e.cbutton.button));
            button_handler_.on_button_down(mask);
            axis_handler_.on_button_down(mask);
        }
        break;
        case SDL_CONTROLLERBUTTONUP: {
            const input_mask_t mask = input_helper::get_input_mask(input_context_,
                                                                   sdl_to_gamepad::to_controller_index(e.cbutton.which),
                                                                   sdl_to_gamepad::to_button((SDL_GameControllerButton) e.cbutton.button));
            button_handler_.on_button_up(mask);
            axis_handler_.on_button_up(mask);
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

        sdl_event_listener.set_callback<sdl_event>(std::bind(&input_manager::on_sdl_event, this, std::placeholders::_1));
        sdl_message_pump::instance().get_event_dispatcher().add_listener<sdl_event>(&sdl_event_listener);
    }

    void input_manager::update() {
        button_handler_.dispatch_events(input_event_dispatcher_);
        axis_handler_.dispatch_events(input_event_dispatcher_);
    }

    void input_manager::exit() {
        sdl_message_pump::instance().get_event_dispatcher().remove_listener<sdl_event>(&sdl_event_listener);
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
    }

    void input_manager::register_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _button) {
        button_handler_.register_button(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _button));
    }

    void input_manager::unregister_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _button) {
        button_handler_.unregister_button(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _button));
    }

    void input_manager::register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _axis) {
        axis_handler_.register_axis(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _axis));
    }

    void input_manager::unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _axis) {
        axis_handler_.unregister_axis(_input_action, input_helper::get_input_mask(_input_context, _controller_index, _axis));
    }

    void input_manager::register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _positive_button, keycode_t _negative_button) {
        axis_handler_.register_button(_input_action,
                                      input_helper::get_input_mask(_input_context, _controller_index, _positive_button),
                                      input_helper::get_input_mask(_input_context, _controller_index, _negative_button));
    }

    void input_manager::unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode_t _positive_button, mkr::keycode_t _negative_button) {
        axis_handler_.unregister_button(_input_action,
                                        input_helper::get_input_mask(_input_context, _controller_index, _positive_button),
                                        input_helper::get_input_mask(_input_context, _controller_index, _negative_button));
    }

    bool input_manager::is_button_down(input_action_t _action) const {
        return button_handler_.is_button_down(_action);
    }

    bool input_manager::is_button_pressed(input_action_t _action) const {
        return button_handler_.is_button_pressed(_action);
    }

    bool input_manager::is_button_up(input_action_t _action) const {
        return button_handler_.is_button_up(_action);
    }

    float input_manager::get_axis_value(input_action_t _action) const {
        return axis_handler_.get_axis_value(_action);
    }
} // mkr
