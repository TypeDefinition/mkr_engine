#pragma once

#include <memory>
#include <flecs.h>
#include <log/log.h>
#include <common/singleton.h>
#include "scene/scene.h"

namespace mkr {
    class scene_manager : public singleton<scene_manager> {
        friend class singleton<scene_manager>;

    private:
        std::unique_ptr<scene> scene_;

        scene_manager() = default;

        virtual ~scene_manager() = default;

    public:
        [[nodiscard]] inline scene* active_scene() { return scene_.get(); }

        void init();

        void update();

        void exit();
    };
}