#pragma once

#include <maths/vector3.h>
#include "application/application.h"
#include "input/input_manager.h"
#include "event/event_listener.h"
#include "component/transform_component.h"
#include "system/system.h"
#include "game/tag/tag.h"
#include "game/input/game_controls.h"

namespace mkr {
    class head_control_system {
    private:
        event_listener input_listener_;
        vector3 rotation_;

    public:
        head_control_system() {
            // Input callback.
            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e || e->state_ != button_state::pressed) { return; }
                if (e->action_ == quit) { application::instance().terminate(); }
                if (e->action_ == look_up) { rotation_.x_ -= 180.0f * application::instance().delta_time(); }
                if (e->action_ == look_down) { rotation_.x_ += 180.0f * application::instance().delta_time(); }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
        }

        ~head_control_system() {
            input_manager::instance().get_event_dispatcher()->remove_listener<button_event>(&input_listener_);
        }

        void operator()(transform_component& _transform, const head_tag& _head) {
            _transform.rotate(quaternion{vector3::x_axis, rotation_.x_ * maths_util::deg2rad});
            rotation_ = vector3::zero;
        }
    };
} // mkr