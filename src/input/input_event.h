#pragma once

#include "event/event.h"
#include "input/input.h"

namespace mkr {
    class input_event : public event {
    protected:
        input_event(input_action _action)
                : action_(_action) {}

    public:
        const input_action action_;
    };

    enum class button_state { down, pressed, up };

    class button_event : public input_event {
    public:
        const button_state state_;

        button_event(input_action _action, button_state _state)
                : input_event(_action), state_(_state) {}
    };

    class cursor_position {
    public:
        float x_, y_, z_;

        cursor_position(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
                : x_(_x), y_(_y), z_(_z) {}
    };

    class click_event : public button_event {
    public:
        const cursor_position cursor_pos_;

        click_event(input_action _action, button_state _state, cursor_position _cursor_pos)
                : button_event(_action, _state), cursor_pos_(_cursor_pos) {}
    };

    enum class axis { x, y, z };

    class axis_event : public input_event {
    public:
        const axis axis_;
        const float value_;

        axis_event(input_action _action, axis _axis, float _value)
                : input_event(_action), axis_(_axis), value_(_value) {}
    };
}