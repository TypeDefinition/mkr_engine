#include "application/application.h"
#include "input/input_manager.h"
#include "graphics/asset_loader.h"
#include "component/transform.h"
#include "component/local_to_world.h"
#include "component/mesh_render.h"
#include "component/light.h"
#include "component/camera.h"
#include "graphics/renderer.h"
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

    void game_scene::update() {
        /*auto q = world_.query_builder<const transform, const g_transform>().term_at(2).optional().up().build();
        q.each([](flecs::entity& _ent, const transform* _child, const g_transform* _parent) {
            matrix4x4 trans;
            quaternion rot;
            if (_parent) {
                trans = _parent->transform_ * _child->transform_matrix();
                rot = _parent->rotation_ * _child->get_rotation();
            } else {
                trans = _child->transform_matrix();
                rot = _child->get_rotation();
            }

            g_transform global_transform;
            global_transform.transform_ = trans;
            global_transform.rotation_ = rot;
            global_transform.position_ = vector3{trans[3][0], trans[3][1], trans[3][2]};
            global_transform.left_ = quaternion::rotate(vector3::x_axis, rot);
            global_transform.up_ = quaternion::rotate(vector3::y_axis, rot);
            global_transform.forward_ = quaternion::rotate(vector3::z_axis, rot);
            _ent.set<g_transform>(global_transform);
        });*/

        auto q = world_.query_builder<const transform, local_to_world, const local_to_world*>()
            .term_at(3).parent()
            .cascade().optional()
            .build();
        q.iter([](flecs::iter& _iter, const transform* _child, local_to_world* _out, const local_to_world* _parent) {
            for (auto i: _iter) {
                matrix4x4 trans = _child[i].transform_matrix();
                quaternion rot = _child[i].get_rotation();
                if (_parent) {
                    trans = _parent->transform_ * trans;
                    rot = _parent->rotation_ * rot;
                }
                _out[i].transform_ = trans;
                _out[i].rotation_ = rot;
                _out[i].position_ = vector3{trans[3][0], trans[3][1], trans[3][2]};
                _out[i].left_ = quaternion::rotate(vector3::x_axis, rot);
                _out[i].up_ = quaternion::rotate(vector3::y_axis, rot);
                _out[i].forward_ = quaternion::rotate(vector3::z_axis, rot);
            }
        });

        world_.progress(application::instance().delta_time());
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
        input_manager::instance().register_button(look_left, input_context_default, controller_index_default, kc_gamepad_button_x);
        input_manager::instance().register_button(look_right, input_context_default, controller_index_default, kc_gamepad_button_b);
        input_manager::instance().register_button(look_up, input_context_default, controller_index_default, kc_gamepad_button_y);
        input_manager::instance().register_button(look_down, input_context_default, controller_index_default, kc_gamepad_button_a);

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
        input_manager::instance().unregister_button(look_left, input_context_default, controller_index_default, kc_gamepad_button_x);
        input_manager::instance().unregister_button(look_right, input_context_default, controller_index_default, kc_gamepad_button_b);
        input_manager::instance().unregister_button(look_up, input_context_default, controller_index_default, kc_gamepad_button_y);
        input_manager::instance().unregister_button(look_down, input_context_default, controller_index_default, kc_gamepad_button_a);

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
        world_.system<const local_to_world, const camera>().each([](const local_to_world& _transform, const camera& _camera) { renderer::instance().update_cameras(_transform, _camera); });
        world_.system<const local_to_world, const light>().each([](const local_to_world& _transform, const light& _light) { renderer::instance().update_lights(_transform, _light); });
        world_.system<const local_to_world, const mesh_render>().each([](const local_to_world& _transform, const mesh_render& _mesh_renderer) { renderer::instance().update_objects(_transform, _mesh_renderer); });
    }

    void game_scene::init_shaders() {
        asset_loader::instance().load_shader_program("skybox", render_pass::skybox, {"./../assets/shaders/skybox.vert"}, {"./../assets/shaders/skybox.frag"});
        asset_loader::instance().load_shader_program("gshader", render_pass::geometry, {"./../assets/shaders/gshader.vert"}, {"./../assets/shaders/gshader.frag"});
        asset_loader::instance().load_shader_program("sshader", render_pass::shadow, {"./../assets/shaders/sshader.vert"}, {"./../assets/shaders/sshader.frag"});
        asset_loader::instance().load_shader_program("lshader", render_pass::lighting, {"./../assets/shaders/lshader.vert"}, {"./../assets/shaders/lshader.frag"});
        asset_loader::instance().load_shader_program("fshader", render_pass::lighting, {"./../assets/shaders/fshader.vert"}, {"./../assets/shaders/fshader.frag"});
        asset_loader::instance().load_shader_program("pshader_invert", render_pass::post_proc, {"./../assets/shaders/pshader.vert"}, {"./../assets/shaders/pshader_invert.frag"});
        asset_loader::instance().load_shader_program("pshader_greyscale", render_pass::post_proc, {"./../assets/shaders/pshader.vert"}, {"./../assets/shaders/pshader_greyscale.frag"});
        asset_loader::instance().load_shader_program("pshader_blur", render_pass::post_proc, {"./../assets/shaders/pshader.vert"}, {"./../assets/shaders/pshader_blur.frag"});
        asset_loader::instance().load_shader_program("pshader_outline", render_pass::post_proc, {"./../assets/shaders/pshader.vert"}, {"./../assets/shaders/pshader_outline.frag"});

        material::lshader_ = asset_loader::instance().get_shader_program("lshader");
        material::gshader_ = asset_loader::instance().get_shader_program("gshader");
        material::sshader_ = asset_loader::instance().get_shader_program("sshader");
        // material::pshaders_.push_back(asset_loader::instance().get_shader_program("pshader_invert"));
        // material::pshaders_.push_back(asset_loader::instance().get_shader_program("pshader_greyscale"));
        // material::pshaders_.push_back(asset_loader::instance().get_shader_program("pshader_blur"));
        // material::pshaders_.push_back(asset_loader::instance().get_shader_program("pshader_outline"));
    }

    void game_scene::init_materials() {
        auto tiles = asset_loader::instance().make_material("tiles");
        tiles->texture_albedo_ = asset_loader::instance().load_texture_2d("tiles_001_albedo", "./../assets/textures/materials/tiles/tiles_001_albedo.png");
        tiles->texture_normal_ = asset_loader::instance().load_texture_2d("tiles_001_normal", "./../assets/textures/materials/tiles/tiles_001_normal.png");
        tiles->texture_displacement_ = asset_loader::instance().load_texture_2d("tiles_001_displacement", "./../assets/textures/materials/tiles/tiles_001_displacement.png");
        tiles->texture_gloss_ = asset_loader::instance().load_texture_2d("tiles_001_gloss", "./../assets/textures/materials/tiles/tiles_001_gloss.png");
        tiles->texture_specular_ = asset_loader::instance().load_texture_2d("tiles_001_specular", "./../assets/textures/materials/tiles/tiles_001_specular.png");
        tiles->texture_scale_ = vector2{1.0f, 1.0f} * 50.0f;
        tiles->displacement_scale_ = 0.1f;
    }

    void game_scene::init_meshes() {
        asset_loader::instance().load_obj("sphere", "./../assets/models/sphere.obj");
        asset_loader::instance().load_obj("cube", "./../assets/models/cube.obj");
        asset_loader::instance().load_obj("plane", "./../assets/models/plane.obj");
    }

    void game_scene::init_levels() {
        // Floor
        transform floor_trans;
        floor_trans.set_scale({100.0f, 1.0f, 100.0f});
        floor_trans.set_position({0.0f, 0.0f, 0.0f});
        mesh_render floor_rend;
        floor_rend.mesh_ = "plane";
        floor_rend.material_ = "tiles";
        world_.entity("floor").set<transform>(floor_trans).set<mesh_render>(floor_rend).add<local_to_world>();

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
        cam.skybox_->shader_ = asset_loader::instance().get_shader_program("skybox");
        cam.skybox_->texture_ = asset_loader::instance().get_texture_cube("skybox_sunset");

        auto body = world_.entity("head").add<transform>().add<body_tag>().add<local_to_world>();
        auto head = world_.entity("body").set<transform>(head_trans).add<head_tag>().set<camera>(cam).add<local_to_world>();
        head.child_of(body);
    }
} // mkr