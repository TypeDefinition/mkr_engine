#pragma once

#include <maths/vector2.h>
#include <maths/vector3.h>
#include "event/event.h"
#include "input/input.h"

namespace mkr {
    enum class input_state {
        start, ongoing, end
    };

    class input_event : public event {
    protected:
        input_event(input_action_t _action, input_state _state)
                : action_(_action), state_(_state) {}

    public:
        /// The input action that was triggered.
        const input_action_t action_;

        /// The input state.
        const input_state state_;

        virtual ~input_event() {}
    };

    class bool_event : public input_event {
    public:
        bool_event(input_action_t _action, input_state _state)
                : input_event(_action, _state) {}

        virtual ~bool_event() {}
    };

    class axis1d_event : public input_event {
    public:
        /// The axis value. Between [-1, 1] for joysticks, and any range for mouse.
        const float value_;

        axis1d_event(input_action_t _action, input_state _state, float _value)
                : input_event(_action, _state), value_(_value) {}

        virtual ~axis1d_event() {}
    };

    class axis2d_event : public input_event {
    public:
        const vector2 value_;

        axis2d_event(input_action_t _action, input_state _state, vector2 _value)
                : input_event(_action, _state), value_(_value) {}

        virtual ~axis2d_event() {}
    };

    class axis3d_event : public input_event {
    public:
        const vector3 value_;

        axis3d_event(input_action_t _action, input_state _state, vector3 _value)
                : input_event(_action, _state), value_(_value) {}

        virtual ~axis3d_event() {}
    };
} // mkr