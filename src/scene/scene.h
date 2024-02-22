#pragma once

#include <utility>
#include <string>
#include <flecs.h>

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
    };
} // mkr