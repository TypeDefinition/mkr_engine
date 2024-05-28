#include "application/application.h"
#include "input/input_manager.h"
#include "graphics/mesh/mesh_manager.h"
#include "graphics/texture/texture_manager.h"
#include "graphics/material/material_manager.h"
#include "graphics/shader/shader_manager.h"
#include "component/transform.h"
#include "component/local_to_world.h"
#include "component/render_mesh.h"
#include "component/light.h"
#include "component/camera.h"
#include "graphics/renderer/graphics_renderer.h"
#include "graphics/shader/forward_shader.h"
#include "graphics/shader/geometry_shader.h"
#include "graphics/shader/lighting_shader.h"
#include "graphics/shader/alpha_weight_shader.h"
#include "graphics/shader/alpha_blend_shader.h"
#include "graphics/shader/post_proc_shader.h"
#include "graphics/shader/skybox_shader.h"
#include "graphics/shader/shadow_2d_shader.h"
#include "graphics/shader/shadow_cubemap_shader.h"
#include "game/tag/tag.h"
#include "game/scene/game_scene.h"

namespace mkr {
    game_scene::game_scene() {}

    game_scene::~game_scene() {}

    void game_scene::init() {
        init_input();
        init_systems();
        init_shaders();
        init_meshes();
        init_materials();
        init_levels();
        init_player();
        init_sphere_gallery();
        init_transparency_gallery();
        // init_shadow_gallery();
    }

    void game_scene::pre_update() {
    }

    void game_scene::post_update() {
    }

    void game_scene::exit() {
        exit_input();
    }

    void game_scene::init_input() {
        // input_manager::instance().set_relative_mouse(true);

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

        // Gamepad Buttons
        input_manager::instance().register_button(look_left, input_context_default, controller_index_default, kc_gamepad_button_dpad_left);
        input_manager::instance().register_button(look_right, input_context_default, controller_index_default, kc_gamepad_button_dpad_right);
        input_manager::instance().register_button(look_up, input_context_default, controller_index_default, kc_gamepad_button_dpad_up);
        input_manager::instance().register_button(look_down, input_context_default, controller_index_default, kc_gamepad_button_dpad_down);

        // Register Axes
        input_manager::instance().register_axis(look_horizontal, input_context_default, controller_index_default, kc_mouse_axis_x);
        input_manager::instance().register_axis(look_vertical, input_context_default, controller_index_default, kc_mouse_axis_y);

        input_manager::instance().register_axis(move_forward, input_context_default, controller_index_default, kc_w, kc_s);
        input_manager::instance().register_axis(move_left, input_context_default, controller_index_default, kc_a, kc_d);

        // Register Clicks
        input_manager::instance().register_click(test_click, input_context_default, controller_index_default, kc_mouse_right);

        // Register Motions
        // input_manager::instance().register_motion(test_motion, input_context_default, controller_index_default, kc_mouse_motion);
    }

    void game_scene::exit_input() {
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

        // Gamepad Buttons
        input_manager::instance().unregister_button(look_left, input_context_default, controller_index_default, kc_gamepad_button_dpad_left);
        input_manager::instance().unregister_button(look_right, input_context_default, controller_index_default, kc_gamepad_button_dpad_right);
        input_manager::instance().unregister_button(look_up, input_context_default, controller_index_default, kc_gamepad_button_dpad_up);
        input_manager::instance().unregister_button(look_down, input_context_default, controller_index_default, kc_gamepad_button_dpad_down);

        // Unregister Axes
        input_manager::instance().unregister_axis(look_horizontal, input_context_default, controller_index_default, kc_mouse_axis_x);
        input_manager::instance().unregister_axis(look_vertical, input_context_default, controller_index_default, kc_mouse_axis_y);

        input_manager::instance().unregister_axis(move_forward, input_context_default, controller_index_default, kc_w, kc_s);
        input_manager::instance().unregister_axis(move_left, input_context_default, controller_index_default, kc_a, kc_d);

        // Unregister Clicks
        input_manager::instance().unregister_click(test_click, input_context_default, controller_index_default, kc_mouse_right);

        // Unregister Motions
        // input_manager::instance().unregister_motion(test_motion, input_context_default, controller_index_default, kc_mouse_motion);
    }

    void game_scene::init_systems() {
        world_.system<transform, const head_tag>().each([&](transform& _trans, const head_tag _head) { hcs_(_trans, _head); });
        world_.system<transform, const body_tag>().each([&](transform& _trans, const body_tag _body) { bcs_(_trans, _body); });
        world_.system<transform, const rotate_tag>().each([](transform& _trans, const rotate_tag _camera) { _trans.rotate(quaternion{vector3::y_axis(), 5.0f * maths_util::deg2rad * application::instance().delta_time()}); });

        // Render systems.
        world_.system<const local_to_world, const camera>().each([](const local_to_world& _transform, const camera& _camera) { graphics_renderer::instance().submit_camera(_transform, _camera); });
        world_.system<const local_to_world, const light>().each([](const local_to_world& _transform, const light& _light) { graphics_renderer::instance().submit_light(_transform, _light); });
        world_.system<const local_to_world, const render_mesh>().each([](const local_to_world& _transform, const render_mesh& _mesh_renderer) { graphics_renderer::instance().submit_mesh(_transform, _mesh_renderer); });
    }

    void game_scene::init_shaders() {
        shader_manager::instance().make_shader<skybox_shader>("skybox",
                                                              {"./assets/shaders/skybox/skybox.vert"},
                                                              {"./assets/shaders/skybox/skybox.frag"});

        shader_manager::instance().make_shader<forward_shader>("forward",
                                                               {"./assets/shaders/forward/forward.vert"},
                                                               {"./assets/shaders/forward/forward.frag",
                                                                "./assets/shaders/include/parallax.frag",
                                                                "./assets/shaders/include/shadow.frag",
                                                                "./assets/shaders/include/light.frag"});

        shader_manager::instance().make_shader<geometry_shader>("geometry",
                                                                {"./assets/shaders/deferred/geometry.vert"},
                                                                {"./assets/shaders/deferred/geometry.frag",
                                                                 "./assets/shaders/include/parallax.frag"});

        shader_manager::instance().make_shader<lighting_shader>("lighting",
                                                                {"./assets/shaders/deferred/lighting.vert"},
                                                                {"./assets/shaders/deferred/lighting.frag",
                                                                 "./assets/shaders/include/shadow.frag",
                                                                 "./assets/shaders/include/light.frag"});

        shader_manager::instance().make_shader<alpha_weight_shader>("alpha_weight",
                                                                    {"./assets/shaders/alpha/alpha_weight.vert"},
                                                                    {"./assets/shaders/alpha/alpha_weight.frag",
                                                                     "./assets/shaders/include/parallax.frag",
                                                                     "./assets/shaders/include/shadow.frag",
                                                                     "./assets/shaders/include/light.frag"});

        shader_manager::instance().make_shader<alpha_blend_shader>("alpha_blend",
                                                                   {"./assets/shaders/alpha/alpha_blend.vert"},
                                                                   {"./assets/shaders/alpha/alpha_blend.frag"});

        shader_manager::instance().make_shader<shadow_2d_shader>("shadow_2d",
                                                                 {"./assets/shaders/shadow/shadow_2d.vert"},
                                                                 {"./assets/shaders/shadow/shadow_2d.frag"});

        shader_manager::instance().make_shader<shadow_cubemap_shader>("shadow_cubemap",
                                                                      {"./assets/shaders/shadow/shadow_cubemap.vert"},
                                                                      {"./assets/shaders/shadow/shadow_cubemap.geom"},
                                                                      {"./assets/shaders/shadow/shadow_cubemap.frag"});

        material::geometry_shader_ = shader_manager::instance().get_shader("geometry");
        material::light_shader_ = shader_manager::instance().get_shader("lighting");
        material::shadow_shader_2d_ = shader_manager::instance().get_shader("shadow_2d");
        material::shadow_shader_cube_ = shader_manager::instance().get_shader("shadow_cubemap");
    }

    void game_scene::init_materials() {
        // Skybox
        texture_manager::instance().make_cubemap("skybox_test", {
            "./assets/textures/skyboxes/test/skybox_test_right.png",
            "./assets/textures/skyboxes/test/skybox_test_left.png",
            "./assets/textures/skyboxes/test/skybox_test_top.png",
            "./assets/textures/skyboxes/test/skybox_test_bottom.png",
            "./assets/textures/skyboxes/test/skybox_test_front.png",
            "./assets/textures/skyboxes/test/skybox_test_back.png",
        });

        texture_manager::instance().make_cubemap("skybox_sunset", {
            "./assets/textures/skyboxes/sunset/skybox_sunset_right.png",
            "./assets/textures/skyboxes/sunset/skybox_sunset_left.png",
            "./assets/textures/skyboxes/sunset/skybox_sunset_top.png",
            "./assets/textures/skyboxes/sunset/skybox_sunset_bottom.png",
            "./assets/textures/skyboxes/sunset/skybox_sunset_front.png",
            "./assets/textures/skyboxes/sunset/skybox_sunset_back.png",
        });

        {
            auto mat = material_manager::instance().make_material("tiles");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("tiles_001_diffuse", "./assets/textures/materials/tiles/tiles_001_albedo.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("tiles_001_normal", "./assets/textures/materials/tiles/tiles_001_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("tiles_001_displacement", "./assets/textures/materials/tiles/tiles_001_displacement.png");
            mat->texture_specular_ = texture_manager::instance().make_texture2d("tiles_001_specular", "./assets/textures/materials/tiles/tiles_001_specular.png");
            mat->texture_scale_ = vector2{1.0f, 1.0f} * 50.0f;
            mat->displacement_scale_ = 0.05f;
        }

        {
            auto mat = material_manager::instance().make_material("pavement");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("pavement_diffuse", "./assets/textures/materials/pavement/pavement_brick_001_albedo.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("pavement_normal", "./assets/textures/materials/pavement/pavement_brick_001_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("pavement_displacement", "./assets/textures/materials/pavement/pavement_brick_001_displacement.png");
            mat->texture_specular_ = texture_manager::instance().make_texture2d("pavement_specular", "./assets/textures/materials/pavement/pavement_brick_001_specular.png");
            mat->texture_scale_ = vector2{1.0f, 1.0f} * 3.0f;
            mat->displacement_scale_ = 0.1f;
        }

        {
            auto mat = material_manager::instance().make_material("brick_wall");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("brick_wall_001_diffuse", "./assets/textures/materials/brick_wall/brick_wall_001_albedo.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("brick_wall_001_normal", "./assets/textures/materials/brick_wall/brick_wall_001_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("brick_wall_001_displacement", "./assets/textures/materials/brick_wall/brick_wall_001_displacement.png");
            mat->texture_specular_ = texture_manager::instance().make_texture2d("brick_wall_001_specular", "./assets/textures/materials/brick_wall/brick_wall_001_specular.png");
            mat->texture_scale_ = vector2{1.0f, 1.0f} * 4.0f;
            mat->displacement_scale_ = 0.05f;
        }

        {
            auto mat = material_manager::instance().make_material("rough_rock");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("rough_rock_diffuse", "./assets/textures/materials/rough_rock/rough_rock_004_albedo.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("rough_rock_normal", "./assets/textures/materials/rough_rock/rough_rock_004_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("rough_rock_displacement", "./assets/textures/materials/rough_rock/rough_rock_004_displacement.png");
            mat->texture_specular_ = texture_manager::instance().make_texture2d("rough_rock_specular", "./assets/textures/materials/rough_rock/rough_rock_004_specular.png");
            mat->texture_scale_ = vector2{1.0f, 1.0f} * 4.0f;
        }

        {
            auto mat = material_manager::instance().make_material("metal_pattern");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("metal_pattern_diffuse", "./assets/textures/materials/metal/metal_pattern_001_albedo.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("metal_pattern_normal", "./assets/textures/materials/metal/metal_pattern_001_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("metal_pattern_displacement", "./assets/textures/materials/metal/metal_pattern_001_displacement.png");
            mat->texture_specular_ = texture_manager::instance().make_texture2d("metal_pattern_specular", "./assets/textures/materials/metal/metal_pattern_001_specular.png");
            mat->texture_scale_ = vector2{1.0f, 1.0f} * 3.0f;
        }

        {
            auto mat = material_manager::instance().make_material("metal_plate");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("metal_plate_diffuse", "./assets/textures/materials/metal/metal_plate_001_albedo.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("metal_plate_normal", "./assets/textures/materials/metal/metal_plate_001_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("metal_plate_displacement", "./assets/textures/materials/metal/metal_plate_001_displacement.png");
            mat->texture_specular_ = texture_manager::instance().make_texture2d("metal_plate_specular", "./assets/textures/materials/metal/metal_plate_001_specular.png");
            mat->texture_scale_ = vector2{1.0f, 1.0f} * 8.0f;
            mat->displacement_scale_ = 0.02f;
        }

        {
            auto mat = material_manager::instance().make_material("green");
            mat->diffuse_colour_ = colour(0.0f, 1.0f, 0.0f, 0.3f);
            mat->alpha_weight_shader_ = shader_manager::instance().get_shader("alpha_weight");
            mat->alpha_blend_shader_ = shader_manager::instance().get_shader("alpha_blend");
            mat->render_path_ = render_path::transparent;
        }

        {
            auto mat = material_manager::instance().make_material("red_op");
            mat->diffuse_colour_ = colour(1.0f, 0.0f, 0.0f, 1.0f);
            mat->render_path_ = render_path::deferred;
        }

        {
            auto mat = material_manager::instance().make_material("green_op");
            mat->diffuse_colour_ = colour(1.0f, 1.0f, 0.0f, 1.0f);
            mat->render_path_ = render_path::deferred;
        }

        {
            auto mat = material_manager::instance().make_material("blue_op");
            mat->diffuse_colour_ = colour(0.0f, 0.0f, 1.0f, 1.0f);
            mat->render_path_ = render_path::deferred;
        }



        {
            auto mat = material_manager::instance().make_material("red");
            mat->diffuse_colour_ = colour{1.0f, 0.0f, 0.0f, 0.3f};
            mat->alpha_weight_shader_ = shader_manager::instance().get_shader("alpha_weight");
            mat->alpha_blend_shader_ = shader_manager::instance().get_shader("alpha_blend");
            mat->render_path_ = render_path::transparent;
        }

        {
            auto mat = material_manager::instance().make_material("blue");
            mat->diffuse_colour_ = colour{0.0f, 0.0f, 1.0f, 0.3f};
            mat->alpha_weight_shader_ = shader_manager::instance().get_shader("alpha_weight");
            mat->alpha_blend_shader_ = shader_manager::instance().get_shader("alpha_blend");
            mat->render_path_ = render_path::transparent;
        }

        {
            auto mat = material_manager::instance().make_material("brick_test");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("brick_diffuse", "./assets/textures/test/brick_diffuse.png");
            mat->texture_normal_ = texture_manager::instance().make_texture2d("brick_normal", "./assets/textures/test/brick_normal.png");
            mat->texture_displacement_ = texture_manager::instance().make_texture2d("brick_displacement", "./assets/textures/test/brick_displacement.png");
            mat->forward_shader_ = shader_manager::instance().get_shader("forward");
            mat->render_path_ = render_path::forward;
        }

        {
            auto mat = material_manager::instance().make_material("window");
            mat->texture_diffuse_ = texture_manager::instance().make_texture2d("window_diffuse", "./assets/textures/test/window.png");
            mat->alpha_weight_shader_ = shader_manager::instance().get_shader("alpha_weight");
            mat->alpha_blend_shader_ = shader_manager::instance().get_shader("alpha_blend");
            mat->render_path_ = render_path::transparent;
        }
    }

    void game_scene::init_meshes() {
        mesh_manager::instance().make_mesh("sphere", "./assets/models/sphere.obj");
        mesh_manager::instance().make_mesh("cube", "./assets/models/cube.obj");
        mesh_manager::instance().make_mesh("plane", "./assets/models/plane.obj");
        mesh_manager::instance().make_mesh("quad", "./assets/models/quad.obj");
        mesh_manager::instance().make_mesh("monkey", "./assets/models/monkey.obj");
        mesh_manager::instance().make_mesh("cone", "./assets/models/cone.obj");
        mesh_manager::instance().make_mesh("torus", "./assets/models/torus.obj");
    }

    void game_scene::init_levels() {
        // Floor
        {
            transform trans;
            trans.set_scale({100.0f, 1.0f, 100.0f});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("plane");
            rend.material_ = material_manager::instance().get_material("tiles");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }

        // Directional Light
        {
            transform light_trans;
            light_trans.set_position({0.0f, 0.0f, 0.0f});
            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis(), 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis(), 45.0f * maths_util::deg2rad);
            light_trans.set_rotation(rotation_x * rotation_y);
            light lt;
            lt.set_shadow_distance(30.0f);
            lt.set_mode(light_mode::directional);
            lt.set_power(1.0f);
            world_.entity().set<transform>(light_trans).set<light>(lt).add<local_to_world>();
        }

        // Point Light
        /*{
            transform light_trans;
            light_trans.set_position({5.0f, 5.0f, -5.0f});
            light lt;
            lt.set_mode(light_mode::point);
            lt.set_power(0.6f);
            world_.entity().set<transform>(light_trans).set<light>(lt).add<local_to_world>();
        }

        // Spotlight
        {
            transform light_trans;
            light_trans.set_position({0.0f, 3.0f, -3.0f});
            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis(), 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis(), 0.0f * maths_util::deg2rad);
            light_trans.set_rotation(rotation_x * rotation_y);
            light lt;
            lt.set_mode(light_mode::spot);
            lt.set_power(0.7f);
            world_.entity().set<transform>(light_trans).set<light>(lt).add<local_to_world>();
        }*/
    }

    void game_scene::init_player() {
        transform head_trans;
        head_trans.set_position({0.0f, 1.7f, 0.0f});

        camera cam;
        cam.skybox_.shader_ = shader_manager::instance().get_shader("skybox");
        cam.skybox_.texture_ = texture_manager::instance().get_cubemap("skybox_sunset");
        // cam.skybox_.texture_ = texture_manager::instance().get_cubemap("skybox_test");

        auto body = world_.entity("head").add<transform>().add<body_tag>().add<local_to_world>();
        auto head = world_.entity("body").set<transform>(head_trans).add<head_tag>().set<camera>(cam).add<local_to_world>();
        head.child_of(body);
    }

    void game_scene::init_sphere_gallery() {
        {
            transform trans;
            trans.set_position({-10.0f, 1.0f, 10.0f});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), maths_util::pi});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
            rend.material_ = material_manager::instance().get_material("metal_pattern");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }

        {
            transform trans;
            trans.set_position({-5.0f, 1.0f, 10.0f});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), maths_util::pi});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
            rend.material_ = material_manager::instance().get_material("metal_plate");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }

        {
            transform trans;
            trans.set_position({0.0f, 1.0f, 10.0f});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), maths_util::pi});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
            rend.material_ = material_manager::instance().get_material("pavement");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }

        {
            transform trans;
            trans.set_position({5.0f, 1.0f, 10.0f});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), maths_util::pi});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
            rend.material_ = material_manager::instance().get_material("brick_wall");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }

        {
            transform trans;
            trans.set_position({10.0f, 1.0f, 10.0f});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), maths_util::pi});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
            rend.material_ = material_manager::instance().get_material("rough_rock");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }
    }

    void game_scene::init_transparency_gallery() {
        {
            transform trans;
            trans.set_position({0.0f, 1.5f, -10.0f});
            trans.set_scale({0.5f, 0.5f, 0.5f});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("monkey");
            rend.material_ = material_manager::instance().get_material("red");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }

        {
            transform trans;
            trans.set_position({0.0f, 1.5f, -10.0f});
            trans.set_scale({1.0f, 1.0f, 1.0f});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("monkey");
            rend.material_ = material_manager::instance().get_material("blue");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }

        {
            transform trans;
            trans.set_position({0.0f, 1.5f, -10.0f});
            trans.set_scale({1.5f, 1.5f, 1.5f});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("monkey");
            rend.material_ = material_manager::instance().get_material("green");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>().add<rotate_tag>();
        }
    }

    void game_scene::init_shadow_gallery() {
        {
            transform trans;
            trans.set_position({0.0f, 6.0f, 0.0f});
            trans.set_rotation(quaternion{vector3::x_axis(), maths_util::deg2rad * 45.0f});
            light lt;
            lt.set_mode(light_mode::spot);
            lt.set_power(1.0f);
            world_.entity().set<transform>(trans).set<light>(lt).add<local_to_world>();
        }

        /*{
            transform trans;
            trans.set_position({0.0f, 10.0f, 0.0f});
            trans.set_rotation(quaternion{vector3::x_axis(), maths_util::deg2rad * 60.0f});
            light lt;
            lt.set_mode(light_mode::spot);
            lt.set_power(1.0f);
            world_.entity().set<transform>(trans).set<light>(lt).add<local_to_world>();
        }*/

        {
            transform trans;
            trans.set_position({10.0f, 10.0f, 0.0f});
            light lt;
            lt.set_mode(light_mode::point);
            lt.set_power(1.0f);
            world_.entity().set<transform>(trans).set<light>(lt).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({-10.0f, 10.0f, 0.0f});
            light lt;
            lt.set_mode(light_mode::point);
            lt.set_power(1.0f);
            world_.entity().set<transform>(trans).set<light>(lt).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({10.0f * std::cos(0.0f * maths_util::deg2rad), 1.0f, 10.0f * std::sin(0.0f * maths_util::deg2rad)});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), maths_util::pi * -0.5f});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("quad");
            rend.material_ = material_manager::instance().get_material("window");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({10.0f * std::cos(60.0f * maths_util::deg2rad), 1.0f, 10.0f * std::sin(60.0f * maths_util::deg2rad)});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), 60.0f * maths_util::deg2rad});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("cube");
            rend.material_ = material_manager::instance().get_material("metal_plate");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({10.0f * std::cos(120.0f * maths_util::deg2rad), 1.0f, 10.0f * std::sin(120.0f * maths_util::deg2rad)});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), 120.0f * maths_util::deg2rad});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
            rend.material_ = material_manager::instance().get_material("rough_rock");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({10.0f * std::cos(180.0f * maths_util::deg2rad), 1.0f, 10.0f * std::sin(180.0f * maths_util::deg2rad)});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), 180.0f * maths_util::deg2rad});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("cone");
            rend.material_ = material_manager::instance().get_material("red_op");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({10.0f * std::cos(240.0f * maths_util::deg2rad), 1.0f, 10.0f * std::sin(240.0f * maths_util::deg2rad)});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), 240.0f * maths_util::deg2rad} * quaternion{vector3::x_axis(), 0.5f * maths_util::pi});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("torus");
            rend.material_ = material_manager::instance().get_material("blue_op");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }

        {
            transform trans;
            trans.set_position({10.0f * std::cos(300.0f * maths_util::deg2rad), 1.0f, 10.0f * std::sin(300.0f * maths_util::deg2rad)});
            trans.set_scale({2.0f, 2.0f, 2.0f});
            trans.set_rotation(quaternion{vector3::y_axis(), 300.0f * maths_util::deg2rad});
            render_mesh rend{};
            rend.mesh_ = mesh_manager::instance().get_mesh("cube");
            rend.material_ = material_manager::instance().get_material("pavement");
            world_.entity().set<transform>(trans).set<render_mesh>(rend).add<local_to_world>();
        }
    }
} // mkr