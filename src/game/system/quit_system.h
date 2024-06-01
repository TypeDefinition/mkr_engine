#pragma once

#include "application/application.h"
#include "input/input_manager.h"
#include "ecs/system/system.h"
#include "game/input/game_controls.h"

namespace mkr {
    class quit_system : public system {
    public:
        quit_system(scene* _scene) : system(_scene, "Quit System") {}

        virtual ~quit_system() {}

        virtual void on_update(float _delta_time) {
            if (input_manager::instance().is_button_pressed(quit)) {
                application::instance().quit();
            }
        }
    };
} // mkr