#pragma once

#include "scene/scene.h"
#include "system/game_controls.h"

namespace mkr {
    class test_scene : public scene {
    private:
        head_control head_control_;
        body_control body_control_;

        void init_controls();

        void init_meshes();

        void init_textures();

        void init_shaders();

        void init_materials();

        void init_systems();

        void exit_controls();

    public:
        virtual void init();

        virtual void update();

        virtual void exit();
    };
}