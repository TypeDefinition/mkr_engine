#include <log/log.h>
#include "scene/test_scene.h"
#include "graphics/asset_loader.h"
#include "graphics/renderer.h"
#include "system/systems.h"
#include "component/camera.h"
#include "component/mesh_renderer.h"
#include "component/transform.h"
#include "component/light.h"

namespace mkr {
    void test_scene::init_controls() {
        // Register Buttons
        input_manager::instance().register_button(quit, input_context_default, controller_index_default, kc_escape);

        input_manager::instance().register_button(move_left, input_context_default, controller_index_default, kc_a);
        input_manager::instance().register_button(move_right, input_context_default, controller_index_default, kc_d);
        input_manager::instance().register_button(move_forward, input_context_default, controller_index_default, kc_w);
        input_manager::instance().register_button(move_backward, input_context_default, controller_index_default, kc_s);

        input_manager::instance().register_button(look_left, input_context_default, controller_index_default, kc_left);
        input_manager::instance().register_button(look_right, input_context_default, controller_index_default, kc_right);
        input_manager::instance().register_button(look_up, input_context_default, controller_index_default, kc_up);
        input_manager::instance().register_button(look_down, input_context_default, controller_index_default, kc_down);
    }

    void test_scene::init_assets() {
        // Load Assets
        {
            asset_loader::instance().load_obj("wall", "/mnt/ZorinWork/mkr_engine/assets/models/wall.obj");
            auto mat = asset_loader::instance().make_material("wall");
            mat->g_shader_ = asset_loader::instance().load_shader_program("gpass", render_pass::geometry, {"/mnt/ZorinWork/mkr_engine/assets/shaders/gpass.vert"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/gpass.frag"});
            mat->l_shader_ = asset_loader::instance().load_shader_program("lpass", render_pass::lighting, {"/mnt/ZorinWork/mkr_engine/assets/shaders/lpass.vert"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/lpass.frag"});
            mat->texture_albedo_ = asset_loader::instance().load_texture_2d("brick_wall_albedo", "/mnt/ZorinWork/mkr_engine/assets/textures/bricks_albedo.jpg");
            mat->texture_normal_ = asset_loader::instance().load_texture_2d("brick_wall_normal", "/mnt/ZorinWork/mkr_engine/assets/textures/bricks_normal.jpg");
            mat->texture_displacement_ = asset_loader::instance().load_texture_2d("brick_wall_displacement", "/mnt/ZorinWork/mkr_engine/assets/textures/bricks_displacement.jpg");
        }

        {
            asset_loader::instance().load_obj("cube", "/mnt/ZorinWork/mkr_engine/assets/models/cube.obj");
            auto mat = asset_loader::instance().make_material("cube");
            mat->render_path_ = render_path::forward_opaque;
            mat->f_shader_ = asset_loader::instance().load_shader_program("fpass", render_pass::forward, {"/mnt/ZorinWork/mkr_engine/assets/shaders/fpass.vert"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/fpass.frag"});
            mat->texture_albedo_ = asset_loader::instance().load_texture_2d("brick_wall_albedo", "/mnt/ZorinWork/mkr_engine/assets/textures/tile_albedo.png");
            mat->texture_normal_ = asset_loader::instance().load_texture_2d("brick_wall_normal", "/mnt/ZorinWork/mkr_engine/assets/textures/tile_normal.png");
            mat->texture_displacement_ = asset_loader::instance().load_texture_2d("brick_wall_displacement", "/mnt/ZorinWork/mkr_engine/assets/textures/tile_displacement.png");
            mat->texture_gloss_ = asset_loader::instance().load_texture_2d("brick_wall_displacement", "/mnt/ZorinWork/mkr_engine/assets/textures/tile_gloss.png");
            mat->texture_specular_ = asset_loader::instance().load_texture_2d("brick_wall_displacement", "/mnt/ZorinWork/mkr_engine/assets/textures/tile_specular.png");
        }
    }

    void test_scene::init_systems() {
        world_.system<transform, const player_head>().each([&](transform& _transform, const player_head& _player_head) { head_control_(_transform, _player_head); });
        world_.system<transform, const player_body>().each([&](transform& _transform, const player_body& _player_body) { body_control_(_transform, _player_body); });

        world_.system<const root>().each(calculate_transforms);
        world_.system<const global_transform, const camera>().each([](const global_transform& _global_transform, const camera& _camera) { renderer::instance().update_cameras(_global_transform, _camera); });
        world_.system<const global_transform, const light>().each([](const global_transform& _global_transform, const light& _light) { renderer::instance().update_lights(_global_transform, _light); });
        world_.system<const global_transform, const mesh_renderer>().each([](const global_transform& _global_transform, const mesh_renderer& _mesh_renderer) { renderer::instance().update_objects(_global_transform, _mesh_renderer); });
    }

    void test_scene::exit_controls() {
        // Unregister Buttons
        input_manager::instance().unregister_button(quit, input_context_default, controller_index_default, kc_escape);

        input_manager::instance().unregister_button(move_left, input_context_default, controller_index_default, kc_a);
        input_manager::instance().unregister_button(move_right, input_context_default, controller_index_default, kc_d);
        input_manager::instance().unregister_button(move_forward, input_context_default, controller_index_default, kc_w);
        input_manager::instance().unregister_button(move_backward, input_context_default, controller_index_default, kc_s);

        input_manager::instance().unregister_button(look_left, input_context_default, controller_index_default, kc_left);
        input_manager::instance().unregister_button(look_right, input_context_default, controller_index_default, kc_right);
        input_manager::instance().unregister_button(look_up, input_context_default, controller_index_default, kc_up);
        input_manager::instance().unregister_button(look_down, input_context_default, controller_index_default, kc_down);
    }

    void test_scene::init() {
        init_controls();
        init_assets();
        init_systems();

        // Scene Root
        auto scene_root = world_.entity().add<root>().add<transform>();

        // Camera
        {
            camera cam;
            cam.skybox_->shader_ = asset_loader::instance().load_shader_program("skybox", render_pass::skybox, {"/mnt/ZorinWork/mkr_engine/assets/shaders/skybox.vert"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/skybox.frag"});
            cam.skybox_->texture_ = asset_loader::instance().load_texture_cube("skybox", {
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/nebula/skybox_nebula_right.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/nebula/skybox_nebula_left.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/nebula/skybox_nebula_top.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/nebula/skybox_nebula_bottom.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/nebula/skybox_nebula_front.png",
                    "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/nebula/skybox_nebula_back.png",
            });

            auto body = world_.entity().add<transform>().add<player_body>();
            body.child_of(scene_root);
            auto head = world_.entity().add<transform>().add<player_head>().set<camera>(cam);
            head.child_of(body);
        }

        // Objects
        for (auto x = -100; x <= 100; x += 20) {
            for (auto y = -100; y <= 100; y += 20) {
                {
                    auto ent = world_.entity();
                    ent.child_of(scene_root);

                    transform trans;
                    trans.set_position({(float)x, (float)y, 5.0f});

                    mesh_renderer renderer;
                    renderer.mesh_ = asset_loader::instance().get_mesh("wall");
                    renderer.material_ = asset_loader::instance().get_material("wall");

                    ent.set<transform>(trans).set<mesh_renderer>(renderer);
                }

                {
                    transform trans;
                    trans.set_position({(float)x, (float)y, 3.0f});
                    light lt;
                    lt.set_power(10.0f);
                    // world_.entity().set<light>(lt).set<transform>(trans).child_of(scene_root);
                }
            }
        }

        {
            auto ent = world_.entity();
            ent.child_of(scene_root);
            transform trans;
            trans.set_position({10.0f, 0.0f, 0.0f});
            trans.set_scale({2.0f, 5.0f, 1.0f});
            trans.rotate(quaternion(vector3::z_axis, 20.0f));
            mesh_renderer renderer;
            renderer.mesh_ = asset_loader::instance().get_mesh("cube");
            renderer.material_ = asset_loader::instance().get_material("cube");
            ent.set<transform>(trans).set<mesh_renderer>(renderer);
        }

        {
            auto ent = world_.entity();
            ent.child_of(scene_root);
            transform trans;
            trans.set_position({3.0f, 0.0f, 30.0f});
            trans.set_scale(vector3{5.0f, 5.0f, 1.0f} * 5.0f);
            trans.rotate(quaternion(vector3::x_axis, 45.0f));
            mesh_renderer renderer;
            renderer.mesh_ = asset_loader::instance().get_mesh("cube");
            renderer.material_ = asset_loader::instance().get_material("cube");
            ent.set<transform>(trans).set<mesh_renderer>(renderer);
        }

        // Lights
        {
            transform trans;
            trans.set_position({0.0f, 0.0f, -10.0f});

            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis, 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis, 45.0f * maths_util::deg2rad);
            trans.set_rotation(rotation_x * rotation_y);

            light lt;
            lt.set_mode(light_mode::directional);
            lt.set_power(0.6f);

            world_.entity().set<light>(lt).set<transform>(trans).child_of(scene_root);
        }
    }

    void test_scene::update() {
        world_.progress(application::instance().delta_time());
    }

    void test_scene::exit() {
        exit_controls();
    }
}