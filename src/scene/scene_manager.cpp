#include "scene/scene_manager.h"
#include "game/scene/game_scene.h"

namespace mkr {
    void scene_manager::init() {
        scene_ = std::make_unique<game_scene>();
        scene_->init();
    }

    void scene_manager::update() {
        scene_->pre_update();
        scene_->update();
        scene_->post_update();
    }

    void scene_manager::exit() {
        scene_->exit();
    }
}