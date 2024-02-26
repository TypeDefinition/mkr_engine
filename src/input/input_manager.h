#pragma once

#include <SDL2/SDL.h>
#include <common/singleton.h>
#include "input/button_handler.h"
#include "input/axis_handler.h"
#include "input/click_handler.h"
#include "input/motion_handler.h"

namespace mkr {
    class input_manager : public singleton<input_manager> {
        friend class singleton<input_manager>;

    private:
        input_context input_context_ = input_context_default;
        event_listener sdl_event_listener;
        event_dispatcher input_event_dispatcher_;

        button_handler button_handler_;
        axis_handler axis_handler_;
        click_handler click_handler_;
        motion_handler motion_handler_;

        input_manager() = default;

        virtual ~input_manager() = default;

        void sdl_event_callback(const event* _event);

    public:
        void init();

        void update();

        void exit();

        inline void set_input_context(input_context _input_context) { input_context_ = _input_context; }

        inline input_context get_input_context() const { return input_context_; }

        inline event_dispatcher* get_event_dispatcher() { return &input_event_dispatcher_; }

        inline const event_dispatcher* get_event_dispatcher() const { return &input_event_dispatcher_; }

        /**
         * While the mouse is in relative mode, the cursor is hidden, the mouse position is constrained to the window,
         * and SDL will report continuous relative mouse motion even if the mouse is at the edge of the window.
         * @param _enabled Set true to enable relative mode, false to disable.
         */
        inline void set_relative_mouse(bool _enabled) { SDL_SetRelativeMouseMode(_enabled ? SDL_TRUE : SDL_FALSE); }

        void register_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);
        void unregister_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);

        void register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);
        void unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);

        void register_click(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);
        void unregister_click(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);

        void register_motion(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);
        void unregister_motion(input_action_t _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);
    };
} // mkr