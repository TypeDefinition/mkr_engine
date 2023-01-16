#pragma once

#include <memory>
#include <flecs.h>
#include "graphics/asset_loader.h"
#include "graphics/renderer.h"
#include "system/cube_control.h"
#include "system/cam_control.h"
#include "component/camera.h"
#include "component/mesh_renderer.h"
#include "component/transform.h"

namespace mkr {
    class scene_loader {
    private:
        cube_control cube_control_;
        cam_control cam_control_;

    public:
        void init_scene(std::shared_ptr<flecs::world> _world) {
            // Register Buttons
            input_manager::instance().register_button(input_quit, input_context_default, controller_index_default, kc_escape);

            input_manager::instance().register_button(input_cube_left, input_context_default, controller_index_default, kc_left);
            input_manager::instance().register_button(input_cube_right, input_context_default, controller_index_default, kc_right);
            input_manager::instance().register_button(input_cube_up, input_context_default, controller_index_default, kc_up);
            input_manager::instance().register_button(input_cube_down, input_context_default, controller_index_default, kc_down);

            input_manager::instance().register_button(input_cam_left, input_context_default, controller_index_default, kc_a);
            input_manager::instance().register_button(input_cam_right, input_context_default, controller_index_default, kc_d);
            input_manager::instance().register_button(input_cam_forward, input_context_default, controller_index_default, kc_w);
            input_manager::instance().register_button(input_cam_backward, input_context_default, controller_index_default, kc_s);

            input_manager::instance().register_button(input_look_left, input_context_default, controller_index_default, kc_left);
            input_manager::instance().register_button(input_look_right, input_context_default, controller_index_default, kc_right);
            input_manager::instance().register_button(input_look_up, input_context_default, controller_index_default, kc_up);
            input_manager::instance().register_button(input_look_down, input_context_default, controller_index_default, kc_down);

            // Load Assets
            asset_loader::instance().load_obj("cube", "/mnt/ZorinWork/mkr_engine/assets/models/cube.obj");
            asset_loader::instance().load_shader_program("forward_shader", {"/mnt/ZorinWork/mkr_engine/assets/shaders/forward.vs"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/forward.fs"});
            asset_loader::instance().load_texture_2d("test_texture", "/mnt/ZorinWork/mkr_engine/assets/textures/test_0.png");

            auto skybox_texture = asset_loader::instance().load_texture_cube("skybox", {
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/Skybox_Test_001_Left_256x256.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/Skybox_Test_001_Right_256x256.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/Skybox_Test_001_Top_256x256.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/Skybox_Test_001_Bottom_256x256.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/Skybox_Test_001_Front_256x256.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/Skybox_Test_001_Back_256x256.png",
            });
            renderer::instance().set_skybox_texture(skybox_texture);

            auto skybox_shader = asset_loader::instance().load_shader_program("skybox", {"/mnt/ZorinWork/mkr_engine/assets/shaders/skybox.vs"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/skybox.fs"});
            renderer::instance().set_skybox_shader(skybox_shader);

            _world->system<transform, mesh_renderer>("mesh_system").each(renderer::instance().mesh_system());
            _world->system<transform, camera>("camera_system").each(renderer::instance().camera_system());

            // Camera
            _world->entity().add<transform>().add<camera>();
            _world->system<transform, camera>("cam_control").each([&](transform& _t, camera& _m) {
                cam_control_(_t, _m);
            });

            // Cube
            {
                transform cube_trans;
                cube_trans.local_position_ = {0.0f, 0.0f, 3.0f};

                mesh_renderer cube_renderer;
                cube_renderer.mesh_ = asset_loader::instance().get_mesh("cube");
                cube_renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
                cube_renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

                _world->entity().set<transform>(cube_trans).set<mesh_renderer>(cube_renderer);

                // Systems
                /* _world->system<transform, mesh_renderer>("cube_control").each([&](transform& _t, mesh_renderer& _m) {
                    cube_control_(_t, _m);
                }); */
            }

            {
                transform cube_trans;
                cube_trans.local_position_ = {-0.5f, -0.5f, 4.0f};

                mesh_renderer cube_renderer;
                cube_renderer.mesh_ = asset_loader::instance().get_mesh("cube");
                cube_renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
                cube_renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

                _world->entity().set<transform>(cube_trans).set<mesh_renderer>(cube_renderer);
            }

            {
                transform cube_trans;
                cube_trans.local_position_ = {3.0f, 1.0f, 3.0f};

                mesh_renderer cube_renderer;
                cube_renderer.mesh_ = asset_loader::instance().get_mesh("cube");
                cube_renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
                cube_renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

                _world->entity().set<transform>(cube_trans).set<mesh_renderer>(cube_renderer);
            }
        }

        void exit_scene(std::shared_ptr<flecs::world> _world) {
            // Unregister Buttons
            input_manager::instance().unregister_button(input_quit, input_context_default, controller_index_default, kc_escape);

            input_manager::instance().unregister_button(input_cube_left, input_context_default, controller_index_default, kc_left);
            input_manager::instance().unregister_button(input_cube_right, input_context_default, controller_index_default, kc_right);
            input_manager::instance().unregister_button(input_cube_up, input_context_default, controller_index_default, kc_up);
            input_manager::instance().unregister_button(input_cube_down, input_context_default, controller_index_default, kc_down);

            input_manager::instance().unregister_button(input_cam_left, input_context_default, controller_index_default, kc_a);
            input_manager::instance().unregister_button(input_cam_right, input_context_default, controller_index_default, kc_d);
            input_manager::instance().unregister_button(input_cam_forward, input_context_default, controller_index_default, kc_w);
            input_manager::instance().unregister_button(input_cam_backward, input_context_default, controller_index_default, kc_s);

            input_manager::instance().unregister_button(input_look_left, input_context_default, controller_index_default, kc_left);
            input_manager::instance().unregister_button(input_look_right, input_context_default, controller_index_default, kc_right);
            input_manager::instance().unregister_button(input_look_up, input_context_default, controller_index_default, kc_up);
            input_manager::instance().unregister_button(input_look_down, input_context_default, controller_index_default, kc_down);
        }
    };
}