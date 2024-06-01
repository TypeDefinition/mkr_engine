#pragma once

#include "ecs/scene/scene.h"

namespace mkr {
    class demo_scene : public scene {
    protected:
        void init_input();
        void exit_input();
        void init_systems();
        void init_shaders();
        void init_meshes();
        void init_materials();
        void init_level();

    public:
        demo_scene();
        virtual ~demo_scene();

        void init() override;
        void exit() override;
    };
} // mkr