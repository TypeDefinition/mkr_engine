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
        const input_action_t action_;

        virtual ~input_event() {}
    };

    enum class button_state { down, pressed, up };

    class button_event : public input_event {
    public:
        const button_state state_;

        button_event(input_action_t _action, button_state _state)
            : input_event(_action), state_(_state) {}
        virtual ~button_event() {}
    };

    class axis_event : public input_event {
    public:
        const float value_;

        axis_event(input_action_t _action, float _value)
            : input_event(_action), value_(_value) {}
        virtual ~axis_event() {}
    };

    class click_event : public button_event {
    public:
        const vector2 position_;

        click_event(input_action_t _action, button_state _state, const vector2& _position)
            : button_event(_action, _state), position_(_position) {}
        virtual ~click_event() {}
    };

    class motion_event : public input_event {
    public:
        const vector2 position_, delta_;

        motion_event(input_action_t _action, const vector2& _position, const vector2& _delta)
            : input_event(_action), position_(_position), delta_(_delta) {}
        virtual ~motion_event() {}
    };
} // mkr