#pragma once

#include <SDL2/SDL.h>
#include <common/singleton.h>
#include "input/keyboard_handler.h"

namespace mkr {
    class input_manager : public singleton<input_manager> {
        friend class singleton<input_manager>;

    private:
        input_context input_context_ = input_context_default;
        event_listener sdl_event_listener;
        event_dispatcher input_event_dispatcher_;
        keyboard_handler keyboard_handler_;

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

        void register_button(input_action _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);

        void unregister_button(input_action _input_action, input_context _input_context, controller_index _controller_index, mkr::keycode _keycode);
    };
}