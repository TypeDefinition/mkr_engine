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
#include "graphics/shader/light_shader.h"
#include "graphics/shader/post_proc_shader.h"
#include "graphics/shader/skybox_shader.h"
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
        input_manager::instance().set_relative_mouse(false);

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

        // Render systems.
        world_.system<const local_to_world, const camera>().each([](const local_to_world& _transform, const camera& _camera) { graphics_renderer::instance().submit_camera(_transform, _camera); });
        world_.system<const local_to_world, const light>().each([](const local_to_world& _transform, const light& _light) { graphics_renderer::instance().submit_light(_transform, _light); });
        world_.system<const local_to_world, const render_mesh>().each([](const local_to_world& _transform, const render_mesh& _mesh_renderer) { graphics_renderer::instance().submit_mesh(_transform, _mesh_renderer); });
    }

    void game_scene::init_shaders() {
        shader_manager::instance().make_shader<skybox_shader>("skybox", {"./../assets/shaders/skybox.vert"}, {"./../assets/shaders/skybox.frag"});
        shader_manager::instance().make_shader<forward_shader>("forward_shader", {"./../assets/shaders/forward.vert"}, {"./../assets/shaders/forward.frag"});
        shader_manager::instance().make_shader<geometry_shader>("geometry_shader", {"./../assets/shaders/geometry.vert"}, {"./../assets/shaders/geometry.frag"});
        shader_manager::instance().make_shader<light_shader>("light_shader", {"./../assets/shaders/light.vert"}, {"./../assets/shaders/light.frag"});
        shader_manager::instance().make_shader<post_proc_shader>("post_proc_invert", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_invert.frag"});
        shader_manager::instance().make_shader<post_proc_shader>("post_proc_greyscale", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_greyscale.frag"});
        shader_manager::instance().make_shader<post_proc_shader>("post_proc_blur", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_blur.frag"});
        shader_manager::instance().make_shader<post_proc_shader>("post_proc_outline", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_outline.frag"});

        material::geometry_shader_ = shader_manager::instance().get_shader("geometry_shader");
        material::light_shader_ = shader_manager::instance().get_shader("light_shader");
    }

    void game_scene::init_materials() {
        // Skybox
        texture_manager::instance().make_texture_cube("skybox_sunset", {
            "./../assets/textures/skyboxes/sunset/skybox_sunset_right.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_left.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_top.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_bottom.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_front.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_back.png",
        });

        // Floor
        auto tiles = material_manager::instance().make_material("tiles");
        tiles->texture_diffuse_ = texture_manager::instance().make_texture_2d("tiles_001_albedo", "./../assets/textures/materials/tiles/tiles_001_albedo.png");
        tiles->texture_normal_ = texture_manager::instance().make_texture_2d("tiles_001_normal", "./../assets/textures/materials/tiles/tiles_001_normal.png");
        tiles->texture_displacement_ = texture_manager::instance().make_texture_2d("tiles_001_displacement", "./../assets/textures/materials/tiles/tiles_001_displacement.png");
        tiles->texture_gloss_ = texture_manager::instance().make_texture_2d("tiles_001_gloss", "./../assets/textures/materials/tiles/tiles_001_gloss.png");
        tiles->texture_specular_ = texture_manager::instance().make_texture_2d("tiles_001_specular", "./../assets/textures/materials/tiles/tiles_001_specular.png");
        tiles->texture_scale_ = vector2{1.0f, 1.0f} * 50.0f;
        tiles->displacement_scale_ = 0.1f;
    }

    void game_scene::init_meshes() {
        mesh_manager::instance().make_mesh("sphere", "./../assets/models/sphere.obj");
        mesh_manager::instance().make_mesh("cube", "./../assets/models/cube.obj");
        mesh_manager::instance().make_mesh("plane", "./../assets/models/plane.obj");
    }

    void game_scene::init_levels() {
        // Floor
        transform floor_trans;
        floor_trans.set_scale({100.0f, 1.0f, 100.0f});
        floor_trans.set_position({0.0f, 0.0f, 0.0f});
        render_mesh floor_rend;
        floor_rend.mesh_ = mesh_manager::instance().get_mesh("plane");
        floor_rend.material_ = material_manager::instance().get_material("tiles");
        world_.entity("floor").set<transform>(floor_trans).set<render_mesh>(floor_rend).add<local_to_world>();

        // Lights
        transform light_trans;
        light_trans.set_position({0.0f, 0.0f, -10.0f});
        quaternion rotation_x, rotation_y;
        rotation_x.set_rotation(vector3::x_axis, 45.0f * maths_util::deg2rad);
        rotation_y.set_rotation(vector3::y_axis, 45.0f * maths_util::deg2rad);
        light_trans.set_rotation(rotation_y * rotation_x);
        light lt;
        lt.set_mode(light_mode::directional);
        lt.set_power(0.6f);
        world_.entity("light").set<transform>(light_trans).set<light>(lt).add<local_to_world>();
    }

    void game_scene::init_player() {
        transform head_trans;
        head_trans.set_position({0.0f, 1.7f, 0.0f});

        camera cam;
        cam.skybox_.shader_ = shader_manager::instance().get_shader("skybox");
        cam.skybox_.texture_ = texture_manager::instance().get_texture_cube("skybox_sunset");

        auto body = world_.entity("head").add<transform>().add<body_tag>().add<local_to_world>();
        auto head = world_.entity("body").set<transform>(head_trans).add<head_tag>().set<camera>(cam).add<local_to_world>();
        head.child_of(body);
    }
} // mkr