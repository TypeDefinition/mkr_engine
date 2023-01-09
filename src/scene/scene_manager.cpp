#include "scene/scene_manager.h"
#include "application/application.h"

namespace mkr {
    void scene_manager::init() {
        world_ = std::make_shared<flecs::world>();
        scene_loader_.init_scene(world_);
    }

    void scene_manager::update() {
        world_->progress(application::instance().delta_time());
    }

    void scene_manager::exit() {
        scene_loader_.exit_scene(world_);
    }
}