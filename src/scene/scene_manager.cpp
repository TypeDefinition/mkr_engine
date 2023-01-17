#include "scene/scene_manager.h"
#include "scene/test_scene.h"
#include <log/log.h>

namespace mkr {
    void scene_manager::init() {
        scene_ = std::make_shared<test_scene>();
        scene_->init();
    }

    void scene_manager::update() {
        scene_->update();
    }

    void scene_manager::exit() {
        scene_->exit();
    }
}