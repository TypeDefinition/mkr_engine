#pragma once

#include <maths/vector2.h>
#include "event/event.h"
#include "input/input.h"

namespace mkr {
    class input_event : public event {
    protected:
        input_event(input_action_t _action)
            : action_(_action) {}

    public:
        /// The input action that was triggered.
        const input_action_t action_;

        virtual ~input_event() {}
    };

    enum class button_state { down, pressed, up };

    class button_event : public input_event {
    public:
        /// The button state. Can be down (just pressed), pressed (just pressed or held), or up (just released).
        const button_state state_;

        button_event(input_action_t _action, button_state _state)
            : input_event(_action), state_(_state) {}
        virtual ~button_event() {}
    };

    class axis_event : public input_event {
    public:
        /// The axis value. Between [-1, 1] for joysticks, and any range for mouse.
        const float value_;

        axis_event(input_action_t _action, float _value)
            : input_event(_action), value_(_value) {}
        virtual ~axis_event() {}
    };
} // mkr