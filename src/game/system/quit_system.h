#pragma once

#include "application/application.h"
#include "input/input_manager.h"
#include "input/input_event.h"
#include "ecs/system/system.h"
#include "game/input/game_controls.h"

namespace mkr {
    class quit_system : public system {
    private:
        event_listener bool_listener_;

    public:
        quit_system(scene* _scene) : system(_scene, "Quit System") {
            bool_listener_.set_callback<bool_event>([](const bool_event* _event) {
                if (_event->action_ == quit) {
                    application::instance().quit();
                }
            });
            input_manager::instance().get_event_dispatcher()->add_listener<bool_event>(&bool_listener_);
        }

        virtual ~quit_system() {
            input_manager::instance().get_event_dispatcher()->remove_listener<bool_event>(&bool_listener_);
        }
    };
} // mkr