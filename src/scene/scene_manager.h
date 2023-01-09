#pragma once

#include <flecs.h>
#include <common/singleton.h>
#include "scene/scene_loader.h"

namespace mkr {
    class scene_manager : public singleton<scene_manager> {
        friend class singleton<scene_manager>;

    private:
        std::shared_ptr<flecs::world> world_;
        scene_loader scene_loader_;

        scene_manager() = default;

        virtual ~scene_manager() = default;

    public:
        [[nodiscard]] inline std::shared_ptr<flecs::world> active_scene() { return world_; }

        void init();

        void update();

        void exit();
    };
}