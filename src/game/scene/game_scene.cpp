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
    }

    void game_scene::pre_update() {
    }

    void game_scene::post_update() {
    }

    void game_scene::exit() {
        exit_input();
    }

    void game_scene::init_input() {
        input_manager::instance().set_relative_mouse(true);

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

        // Render systems.
        world_.system<const local_to_world, const camera>().each([](const local_to_world& _transform, const camera& _camera) { graphics_renderer::instance().submit_camera(_transform, _camera); });
        world_.system<const local_to_world, const light>().each([](const local_to_world& _transform, const light& _light) { graphics_renderer::instance().submit_light(_transform, _light); });
        world_.system<const local_to_world, const render_mesh>().each([](const local_to_world& _transform, const render_mesh& _mesh_renderer) { graphics_renderer::instance().submit_mesh(_transform, _mesh_renderer); });
    }

    void game_scene::init_shaders() {
        // shader_manager::instance().make_shader<skybox_shader>("skybox", {"./../assets/shaders/skybox.vert"}, {"./../assets/shaders/skybox.frag"});
        shader_manager::instance().make_shader<forward_shader>("forward", {"./../assets/shaders/forward.vert"}, {"./../assets/shaders/forward.frag"});
        // shader_manager::instance().make_shader<geometry_shader>("geometry", {"./../assets/shaders/geometry.vert"}, {"./../assets/shaders/geometry.frag"});
        // shader_manager::instance().make_shader<light_shader>("light", {"./../assets/shaders/light.vert"}, {"./../assets/shaders/light.frag"});
        // shader_manager::instance().make_shader<post_proc_shader>("post_proc_invert", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_invert.frag"});
        // shader_manager::instance().make_shader<post_proc_shader>("post_proc_greyscale", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_greyscale.frag"});
        // shader_manager::instance().make_shader<post_proc_shader>("post_proc_blur", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_blur.frag"});
        // shader_manager::instance().make_shader<post_proc_shader>("post_proc_outline", {"./../assets/shaders/post_proc.vert"}, {"./../assets/shaders/post_proc_outline.frag"});
        shader_manager::instance().make_shader<shadow_2d_shader>("shadow_2d", {"./../assets/shaders/shadow_2d.vert"}, {"./../assets/shaders/shadow_2d.frag"});
        shader_manager::instance().make_shader<shadow_cubemap_shader>("shadow_cubemap", {"./../assets/shaders/shadow_cubemap.vert"}, {"./../assets/shaders/shadow_cubemap.geom"}, {"./../assets/shaders/shadow_cubemap.frag"});

        // material::geometry_shader_ = shader_manager::instance().get_shader("geometry");
        // material::light_shader_ = shader_manager::instance().get_shader("light");
        material::shadow_shader_2d_ = shader_manager::instance().get_shader("shadow_2d");
        material::shadow_shader_cube_ = shader_manager::instance().get_shader("shadow_cubemap");
    }

    void game_scene::init_materials() {
        // Skybox
        texture_manager::instance().make_cubemap("skybox_sunset", {
            "./../assets/textures/skyboxes/sunset/skybox_sunset_right.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_left.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_top.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_bottom.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_front.png",
            "./../assets/textures/skyboxes/sunset/skybox_sunset_back.png",
        });

        // Floor
        auto tiles = material_manager::instance().make_material("tiles");
        tiles->texture_diffuse_ = texture_manager::instance().make_texture2d("tiles_001_albedo", "./../assets/textures/materials/tiles/tiles_001_albedo.png");
        tiles->texture_normal_ = texture_manager::instance().make_texture2d("tiles_001_normal", "./../assets/textures/materials/tiles/tiles_001_normal.png");
        tiles->texture_displacement_ = texture_manager::instance().make_texture2d("tiles_001_displacement", "./../assets/textures/materials/tiles/tiles_001_displacement.png");
        tiles->texture_gloss_ = texture_manager::instance().make_texture2d("tiles_001_gloss", "./../assets/textures/materials/tiles/tiles_001_gloss.png");
        tiles->texture_specular_ = texture_manager::instance().make_texture2d("tiles_001_specular", "./../assets/textures/materials/tiles/tiles_001_specular.png");
        tiles->texture_scale_ = vector2{1.0f, 1.0f} * 50.0f;
        tiles->displacement_scale_ = 0.1f;
        tiles->forward_shader_ = shader_manager::instance().get_shader("forward");
        tiles->render_path_ = render_path::forward_opaque;

        // Cube
        auto brick_wall = material_manager::instance().make_material("brick_wall");
        brick_wall->texture_diffuse_ = texture_manager::instance().make_texture2d("brick_wall_001_albedo", "./../assets/textures/materials/brick_wall/brick_wall_001_albedo.png");
        brick_wall->texture_normal_ = texture_manager::instance().make_texture2d("brick_wall_001_normal", "./../assets/textures/materials/brick_wall/brick_wall_001_normal.png");
        brick_wall->texture_displacement_ = texture_manager::instance().make_texture2d("brick_wall_001_displacement", "./../assets/textures/materials/brick_wall/brick_wall_001_displacement.png");
        brick_wall->texture_gloss_ = texture_manager::instance().make_texture2d("brick_wall_001_gloss", "./../assets/textures/materials/brick_wall/brick_wall_001_gloss.png");
        brick_wall->texture_specular_ = texture_manager::instance().make_texture2d("brick_wall_001_specular", "./../assets/textures/materials/brick_wall/brick_wall_001_specular.png");
        brick_wall->displacement_scale_ = 0.05f;
        brick_wall->forward_shader_ = shader_manager::instance().get_shader("forward");
        brick_wall->render_path_ = render_path::forward_opaque;

        // Sphere
        auto green = material_manager::instance().make_material("green");
        green->diffuse_colour_ = colour::green();
        green->forward_shader_ = shader_manager::instance().get_shader("forward");
        green->render_path_ = render_path::forward_opaque;
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
        render_mesh floor_rend{};
        floor_rend.mesh_ = mesh_manager::instance().get_mesh("plane");
        floor_rend.material_ = material_manager::instance().get_material("tiles");
        world_.entity("floor").set<transform>(floor_trans).set<render_mesh>(floor_rend).add<local_to_world>();

        // Cube
        // transform cube_trans;
        // cube_trans.set_position({5.0f, 0.5f, 5.0f});
        // render_mesh cube_rend{};
        // cube_rend.mesh_ = mesh_manager::instance().get_mesh("cube");
        // cube_rend.material_ = material_manager::instance().get_material("brick_wall");
        // world_.entity("cube").set<transform>(cube_trans).set<render_mesh>(cube_rend).add<local_to_world>();

        // Sphere
        transform sphere_trans;
        sphere_trans.set_position({0.0f, 0.5f, 0.0f});
        render_mesh sphere_rend{};
        sphere_rend.mesh_ = mesh_manager::instance().get_mesh("sphere");
        sphere_rend.material_ = material_manager::instance().get_material("green");
        world_.entity("sphere").set<transform>(sphere_trans).set<render_mesh>(sphere_rend).add<local_to_world>();

        // Directional Light
        {
            transform light_trans;
            light_trans.set_position({0.0f, 0.0f, 0.0f});
            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis(), 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis(), 45.0f * maths_util::deg2rad);
            light_trans.set_rotation(rotation_x * rotation_y);
            light lt;
            lt.set_mode(light_mode::directional);
            lt.set_power(0.1f);
            world_.entity().set<transform>(light_trans).set<light>(lt).add<local_to_world>();
        }

        // Point Light
        {
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
            lt.set_power(1.0f);
            world_.entity().set<transform>(light_trans).set<light>(lt).add<local_to_world>();
        }
    }

    void game_scene::init_player() {
        transform head_trans;
        head_trans.set_position({0.0f, 1.7f, 0.0f});

        camera cam;
        cam.skybox_.shader_ = shader_manager::instance().get_shader("skybox");
        cam.skybox_.texture_ = texture_manager::instance().get_cubemap("skybox_sunset");

        auto body = world_.entity("head").add<transform>().add<body_tag>().add<local_to_world>();
        auto head = world_.entity("body").set<transform>(head_trans).add<head_tag>().set<camera>(cam).add<local_to_world>();
        head.child_of(body);
    }
} // mkr