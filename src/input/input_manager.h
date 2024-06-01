#pragma once

#include <SDL2/SDL.h>
#include <common/singleton.h>
#include "application/sdl_message_pump.h"
#include "input/button_handler.h"
#include "input/axis_handler.h"
#include "input/keyboard_button.h"
#include "input/mouse_button.h"
#include "input/mouse_axis.h"
#include "input/gamepad_button.h"
#include "input/gamepad_axis.h"

namespace mkr {
    /**
     * The input manager is an interface for the user to register input actions to an input event.
     * This abstracts away the need for the game code to know the exact type of input device that is being used.
     * The 4 event types are button, axis, click and motion.
     */
    class input_manager : public singleton<input_manager> {
        friend class singleton<input_manager>;

    private:
        input_context input_context_ = input_context_default;
        event_listener sdl_event_listener;
        event_dispatcher input_event_dispatcher_;

        button_handler button_handler_;
        axis_handler axis_handler_;

        void on_sdl_event(const sdl_event* _event);

    public:
        input_manager() = default;

        virtual ~input_manager() = default;

        void init();

        void update();

        void exit();

        /**
         * Set the current input context.
         * Refer to input.h for more information.
         * @param _input_context The input context to set to.
         * @see input.h
         */
        inline void set_input_context(input_context _input_context) { input_context_ = _input_context; }

        /**
         * Get the current input context.
         * Refer to input.h for more information.
         * @return The current input context.
         * @see input.h
         */
        inline input_context get_input_context() const { return input_context_; }

        /**
         * Get the event dispatcher. Input events will be dispatched via this dispatcher.
         * @return The event dispatcher.
         * @see input_event.h
         */
        inline event_dispatcher* get_event_dispatcher() { return &input_event_dispatcher_; }

        /**
         * Get the event dispatcher. Input events will be dispatched via this dispatcher.
         * @return The event dispatcher.
         * @see input_event.h
         */
        inline const event_dispatcher* get_event_dispatcher() const { return &input_event_dispatcher_; }

        /**
         * In relative mode, the cursor is hidden, the mouse position is constrained to the window,
         * but the continuous relative mouse motion will still be reported even if the mouse is at the edge of the window.
         * @param _enabled Set true to enable relative mode, false to disable.
         */
        inline void set_relative_mouse(bool _enabled) { SDL_SetRelativeMouseMode(_enabled ? SDL_TRUE : SDL_FALSE); }

        /**
         * Register an input action to an input context, controller index and button.
         * Refer to input.h and keycode.h for more information.
         * @param _input_action The input action that will be triggered when the following conditions are met.
         * @param _input_context The context that must be met for this action to be triggered.
         * @param _controller_index The controller index that must be met for this action to be triggered.
         * @param _button The physical button that must be pressed for this action to be triggered.
         * @see input.h, input_event.h
         */
        void register_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _button);

        /**
         * Unregister an input action to an input context, controller index and button.
         * Refer to input.h and keycode.h for more information.
         * @param _input_action The input action that will be triggered when the following conditions are met.
         * @param _input_context The context that must be met for this action to be triggered.
         * @param _controller_index The controller index that must be met for this action to be triggered.
         * @param _button The physical button that must be pressed for this action to be triggered.
         * @see input.h, input_event.h
         */
        void unregister_button(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _button);

        /**
         * Register an input action to an input context, controller index and axis.
         * Refer to input.h and keycode.h for more information.
         * @param _input_action The input action that will be triggered when the following conditions are met.
         * @param _input_context The context that must be met for this action to be triggered.
         * @param _controller_index The controller index that must be met for this action to be triggered.
         * @param _axis The physical axis that must be moved for this action to be triggered.
         * @see input.h, input_event.h
         */
        void register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _axis);

        /**
         * Unregister an input action to an input context, controller index and axis.
         * Refer to input.h and keycode.h for more information.
         * @param _input_action The input action that will be triggered when the following conditions are met.
         * @param _input_context The context that must be met for this action to be triggered.
         * @param _controller_index The controller index that must be met for this action to be triggered.
         * @param _axis The physical axis that must be moved for this action to be triggered.
         * @see input.h, input_event.h
         */
        void unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _axis);

        /**
         * Register an input action to an input context, controller index and axis.
         * In this version, the axis is represented by 2 buttons presses rather than an actual axis movement such as a joystick or mouse.
         * Refer to input.h and keycode.h for more information.
         * @param _input_action The input action that will be triggered when the following conditions are met.
         * @param _input_context The context that must be met for this action to be triggered.
         * @param _controller_index The controller index that must be met for this action to be triggered.
         * @param _positive_button The physical button representing 1 on the axis.
         * @param _negative_button The physical button representing -1 on the axis.
         * @see input.h, input_event.h
         */
        void register_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _positive_button, keycode_t _negative_button);

        /**
         * Unregister an input action to an input context, controller index and axis.
         * In this version, the axis is represented by 2 buttons presses rather than an actual axis movement such as a joystick or mouse.
         * Refer to input.h and keycode.h for more information.
         * @param _input_action The input action that will be triggered when the following conditions are met.
         * @param _input_context The context that must be met for this action to be triggered.
         * @param _controller_index The controller index that must be met for this action to be triggered.
         * @param _positive_button The physical button press representing 1 on the axis.
         * @param _negative_button The physical button press representing -1 on the axis.
         * @see input.h, input_event.h
         */
        void unregister_axis(input_action_t _input_action, input_context _input_context, controller_index _controller_index, keycode_t _positive_button, keycode_t _negative_button);

        bool is_button_down(input_action_t _action) const;

        bool is_button_pressed(input_action_t _action) const;

        bool is_button_up(input_action_t _action) const;

        float get_axis_value(input_action_t _action) const;
    };
} // mkr
