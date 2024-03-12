#pragma once

#include "scene/scene.h"
#include "game/input/game_controls.h"
#include "game/system/head_control_system.h"
#include "game/system/body_control_system.h"
#include "game/system/motion_test_system.h"

namespace mkr {
    class game_scene : public scene {
    private:
        head_control_system hcs_;
        body_control_system bcs_;
        motion_test_system mts_;

    protected:
        void init_input();
        void exit_input();
        void init_systems();
        void init_shaders();
        void init_meshes();
        void init_materials();
        void init_levels();
        void init_player();

    public:
        game_scene();
        virtual ~game_scene();

        void init() override;
        void pre_update() override;
        void post_update() override;
        void exit() override;
    };
} // mkr