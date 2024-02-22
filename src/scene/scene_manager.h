#pragma once

#include <memory>
#include <flecs.h>
#include <common/singleton.h>
#include "scene/scene.h"

namespace mkr {
    class scene_manager : public singleton<scene_manager> {
        friend class singleton<scene_manager>;

    private:
        std::shared_ptr<scene> scene_;

        scene_manager() = default;

        virtual ~scene_manager() = default;

    public:
        [[nodiscard]] inline std::shared_ptr<scene> active_scene() { return scene_; }

        void init();

        void update();

        void exit();
    };
}