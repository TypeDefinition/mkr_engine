#pragma once

#include "ecs/system/system.h"
#include "ecs/scene/scene.h"
#include "ecs/component/local_to_world.h"
#include "ecs/component/light.h"
#include "ecs/component/renderable.h"
#include "ecs/component/camera.h"
#include "graphics/renderer/graphics_renderer.h"

namespace mkr {
    class renderable_system : public system {
    public:
        renderable_system(scene *_scene)
                : system(_scene, "Renderable System") {}

        virtual ~renderable_system() {}

        virtual void on_update(float _delta_time) {
            auto cams = scene_->entities_with<camera, local_to_world>();
            for (auto ent : cams) {
                graphics_renderer::instance().submit_camera(*scene_->get_component<local_to_world>(ent), *scene_->get_component<camera>(ent));
            }

            auto lights = scene_->entities_with<light, local_to_world>();
            for (auto ent : lights) {
                graphics_renderer::instance().submit_light(*scene_->get_component<local_to_world>(ent), *scene_->get_component<light>(ent));
            }

            auto renderables = scene_->entities_with<renderable, local_to_world>();
            for (auto ent : renderables) {
                graphics_renderer::instance().submit_mesh(*scene_->get_component<local_to_world>(ent), *scene_->get_component<renderable>(ent));
            }
        }
    };
}
