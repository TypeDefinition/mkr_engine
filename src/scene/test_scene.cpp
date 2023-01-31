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
        asset_loader::instance().load_obj("cube", "/mnt/ZorinWork/mkr_engine/assets/models/cube.obj");
        asset_loader::instance().load_shader_program("forward_shader", render_pass::forward, {"/mnt/ZorinWork/mkr_engine/assets/shaders/forward.vert"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/forward.frag"});
        asset_loader::instance().load_texture_2d("test_texture", "/mnt/ZorinWork/mkr_engine/assets/textures/test.png");

        renderer::instance().set_skybox_shader(asset_loader::instance().load_shader_program("skybox", render_pass::skybox, {"/mnt/ZorinWork/mkr_engine/assets/shaders/skybox.vert"}, {"/mnt/ZorinWork/mkr_engine/assets/shaders/skybox.frag"}));
        renderer::instance().set_skybox_texture(asset_loader::instance().load_texture_cube("skybox", {
                "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/skybox_test_right.png",
                "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/skybox_test_left.png",
                "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/skybox_test_top.png",
                "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/skybox_test_bottom.png",
                "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/skybox_test_front.png",
                "/mnt/ZorinWork/mkr_engine/assets/textures/skyboxes/skybox_test_back.png",
        }));
    }

    void test_scene::init_systems() {
        world_.system<transform, const player_head>().each([&](transform& _transform, const player_head& _player_head) { head_control_(_transform, _player_head); });
        world_.system<transform, const player_body>().each([&](transform& _transform, const player_body& _player_body) { body_control_(_transform, _player_body); });

        world_.system<transform, mesh_renderer>().each([&](transform& _transform, mesh_renderer& _mesh_renderer) {
            quaternion rotation(vector3::z_axis, application::instance().delta_time() * maths_util::deg2rad * 30.0f);
            _transform.rotate(rotation);
        });

        world_.system<const root>().each(calculate_transforms);
        world_.system<const global_transform, const camera>().each([](const global_transform& _global_transform, const camera& _camera) { renderer::instance().prep_cameras(_global_transform, _camera); });
        world_.system<const global_transform, const light>().each([](const global_transform& _global_transform, const light& _light) { renderer::instance().prep_lights(_global_transform, _light); });
        world_.system<const global_transform, const mesh_renderer>().each([](const global_transform& _global_transform, const mesh_renderer& _mesh_renderer) { renderer::instance().sort_meshes(_global_transform, _mesh_renderer); });
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

        // Camera
        auto body = world_.entity().add<root>().add<transform>().add<player_body>();
        auto head = world_.entity().add<transform>().add<player_head>().add<camera>();
        head.child_of(body);

        auto entity0 = world_.entity().add<root>();
        {
            transform trans;
            trans.set_position({0.0f, -1.0f, 5.0f});

            mesh_renderer renderer;
            renderer.mesh_ = asset_loader::instance().get_mesh("cube");
            renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
            renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

            entity0.set<transform>(trans).set<mesh_renderer>(renderer);
        }

        auto entity1 = world_.entity();
        {
            transform trans;
            trans.set_position({9.0f, 0.0f, 0.0f});

            mesh_renderer renderer;
            renderer.mesh_ = asset_loader::instance().get_mesh("cube");
            renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
            renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

            entity1.set<transform>(trans).set<mesh_renderer>(renderer);
        }

        auto entity2 = world_.entity();
        {
            transform trans;
            trans.set_position({3.0f, 0.0f, 0.0f});

            mesh_renderer renderer;
            renderer.mesh_ = asset_loader::instance().get_mesh("cube");
            renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
            renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

            entity2.set<transform>(trans).set<mesh_renderer>(renderer);
        }

        entity2.child_of(entity1);
        entity1.child_of(entity0);

        /*int count = 20;
        for (int x = 0; x <= count; x += 2) {
            for (int y = 0; y <= count; y += 2) {
                for (int z = 0; z <= count; z += 2) {
                    auto cube = world_.entity();

                    transform trans;
                    trans.set_position({(float)x, (float)y, (float)z});

                    mesh_renderer renderer;
                    renderer.mesh_ = asset_loader::instance().get_mesh("cube");
                    renderer.texture_2d_ = asset_loader::instance().get_texture_2d("test_texture");
                    renderer.shader_ = asset_loader::instance().get_shader_program("forward_shader");

                    cube.set<transform>(trans).set<mesh_renderer>(renderer).add<root>();
                }
            }
        }*/

        // Lights
        {
            transform trans;
            trans.set_position({0.0f, 5.0f, 0.0f});

            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis, 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis, 45.0f * maths_util::deg2rad);
            trans.set_rotation(rotation_x * rotation_y);

            light lt;
            // lt.set_mode(light_mode::directional);
            // lt.set_power(0.5f);

            world_.entity().set<light>(lt).set<transform>(trans).add<root>();
        }
    }

    void test_scene::update() {
        world_.progress(application::instance().delta_time());
    }

    void test_scene::exit() {
        exit_controls();
    }
}