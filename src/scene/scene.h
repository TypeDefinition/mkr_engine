#pragma once

#include <flecs.h>
#include "component/tag.h"

namespace mkr {
    class scene {
    protected:
        flecs::world world_;

    public:
        scene() {}

        virtual ~scene() {}

        virtual void init() = 0;

        virtual void update() = 0;

        virtual void exit() = 0;

        inline flecs::world& world() { return world_; }

        inline const flecs::world& world() const { return world_; }
    };
}