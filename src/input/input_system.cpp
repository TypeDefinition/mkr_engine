#include <log/log.h>
#include "application/sdl_event_system.h"
#include "input/input_system.h"

namespace mkr {
    void input_system::sdl_event_callback(const event* _event) {
        const auto& e = static_cast<const sdl_event*>(_event)->sdl_event_;
        switch (e.type) {
            case SDL_KEYDOWN: {
                // Keyboard defaults to controller_index_0.
                input_mask mask = input::get_input_mask(input_context_, controller_index_0, (keycode) e.key.keysym.sym);
                keyboard_handler_.on_key_down(mask);
            }
                break;
            case SDL_KEYUP: {
                // Keyboard defaults to controller_index_0.
                input_mask mask = input::get_input_mask(input_context_, controller_index_0, (keycode) e.key.keysym.sym);
                keyboard_handler_.on_key_up(mask);
            }
                break;
            default:
                break;
        }
    }

    void input_system::init() {
        if (0 != SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC)) {
            const std::string err_msg = "SDL_INIT_GAMECONTROLLER or SDL_INIT_JOYSTICK or SDL_INIT_HAPTIC failed";
            mkr::log::error(err_msg);
            throw std::runtime_error(err_msg);
        }

        sdl_event_listener.set_callback(std::bind(&input_system::sdl_event_callback, this, std::placeholders::_1));
        sdl_event_system::instance().get_event_dispatcher()->add_listener<sdl_event>(&sdl_event_listener);
    }

    void input_system::update() {
        keyboard_handler_.dispatch_events(input_event_dispatcher_);
    }

    void input_system::exit() {
        sdl_event_system::instance().get_event_dispatcher()->remove_listener<sdl_event>(&sdl_event_listener);
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
    }

    void input_system::register_button(input_name _input_name, input_mask _input_mask) {
        keyboard_handler_.register_button(_input_name, _input_mask);
    }

    void input_system::unregister_button(input_name _input_name, input_mask _input_mask) {
        keyboard_handler_.unregister_button(_input_name, _input_mask);
    }
}