#include "application/application.h"
#include "input/input_manager.h"
#include "graphics/asset_loader.h"
#include "component/transform_component.h"
#include "component/render_component.h"
#include "component/light_component.h"
#include "component/camera_component.h"
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

    void game_scene::update() {
        /*auto q = world_.query_builder<const transform_component, const g_transform_component>().term_at(2).optional().up().build();
        q.each([](flecs::entity& _ent, const transform_component* _child, const g_transform_component* _parent) {
            matrix4x4 trans;
            quaternion rot;
            if (_parent) {
                trans = _parent->transform_ * _child->transform_matrix();
                rot = _parent->rotation_ * _child->get_rotation();
            } else {
                trans = _child->transform_matrix();
                rot = _child->get_rotation();
            }

            g_transform_component global_transform;
            global_transform.transform_ = trans;
            global_transform.rotation_ = rot;
            global_transform.position_ = vector3{trans[3][0], trans[3][1], trans[3][2]};
            global_transform.left_ = quaternion::rotate(vector3::x_axis, rot);
            global_transform.up_ = quaternion::rotate(vector3::y_axis, rot);
            global_transform.forward_ = quaternion::rotate(vector3::z_axis, rot);
            _ent.set<g_transform_component>(global_transform);
        });*/

        auto q = world_.query_builder<transform_component, const transform_component*>()
            .term_at(2).parent()
            .up().optional()
            .build();
        q.iter([](flecs::iter& _iter, transform_component* _child, const transform_component* _parent) {
            for (auto i: _iter) {
                matrix4x4 trans = _child[i].transform_matrix();
                quaternion rot = _child[i].get_rotation();
                if (_parent) {
                    trans = _parent->world_transform_ * trans;
                    rot = _parent->world_rotation_ * rot;
                }
                _child[i].world_transform_ = trans;
                _child[i].world_rotation_ = rot;
                _child[i].world_position_ = vector3{trans[3][0], trans[3][1], trans[3][2]};
                _child[i].world_left_ = quaternion::rotate(vector3::x_axis, rot);
                _child[i].world_up_ = quaternion::rotate(vector3::y_axis, rot);
                _child[i].world_forward_ = quaternion::rotate(vector3::z_axis, rot);
            }
        });

        world_.progress(application::instance().delta_time());
    }

    void game_scene::exit() {
        exit_input();
    }

    void game_scene::init_input() {
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

        input_manager::instance().register_button(debug_mode_off, input_context_default, controller_index_default, kc_0);
        input_manager::instance().register_button(debug_mode_position, input_context_default, controller_index_default, kc_1);
        input_manager::instance().register_button(debug_mode_normal, input_context_default, controller_index_default, kc_2);
        input_manager::instance().register_button(debug_mode_albedo, input_context_default, controller_index_default, kc_3);
        input_manager::instance().register_button(debug_mode_specular, input_context_default, controller_index_default, kc_4);
        input_manager::instance().register_button(debug_mode_gloss, input_context_default, controller_index_default, kc_5);
        input_manager::instance().register_button(debug_mode_material, input_context_default, controller_index_default, kc_6);
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

        input_manager::instance().unregister_button(debug_mode_off, input_context_default, controller_index_default, kc_0);
        input_manager::instance().unregister_button(debug_mode_position, input_context_default, controller_index_default, kc_1);
        input_manager::instance().unregister_button(debug_mode_normal, input_context_default, controller_index_default, kc_2);
        input_manager::instance().unregister_button(debug_mode_albedo, input_context_default, controller_index_default, kc_3);
        input_manager::instance().unregister_button(debug_mode_specular, input_context_default, controller_index_default, kc_4);
        input_manager::instance().unregister_button(debug_mode_gloss, input_context_default, controller_index_default, kc_5);
        input_manager::instance().unregister_button(debug_mode_material, input_context_default, controller_index_default, kc_6);
    }

    void game_scene::init_systems() {
        world_.system<transform_component, const head_tag>().each([&](transform_component& _trans, const head_tag _head) { hcs_(_trans, _head); });
        world_.system<transform_component, const body_tag>().each([&](transform_component& _trans, const body_tag _body) { bcs_(_trans, _body); });
        world_.system<const transform_component, const camera_component>().each([](const transform_component& _transform, const camera_component& _camera) { renderer::instance().update_cameras(_transform, _camera); });
        world_.system<const transform_component, const light_component>().each([](const transform_component& _transform, const light_component& _light) { renderer::instance().update_lights(_transform, _light); });
        world_.system<const transform_component, const render_component>().each([](const transform_component& _transform, const render_component& _mesh_renderer) { renderer::instance().update_objects(_transform, _mesh_renderer); });
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
        transform_component floor_trans;
        floor_trans.set_scale({100.0f, 1.0f, 100.0f});
        floor_trans.set_position({0.0f, 0.0f, 0.0f});
        render_component floor_rend;
        floor_rend.mesh_ = "plane";
        floor_rend.material_ = "tiles";
        world_.entity("floor").set<transform_component>(floor_trans).set<render_component>(floor_rend);

        // Lights
        transform_component light_trans;
        light_trans.set_position({0.0f, 0.0f, -10.0f});
        quaternion rotation_x, rotation_y;
        rotation_x.set_rotation(vector3::x_axis, 45.0f * maths_util::deg2rad);
        rotation_y.set_rotation(vector3::y_axis, 45.0f * maths_util::deg2rad);
        light_trans.set_rotation(rotation_y * rotation_x);
        light_component lt;
        lt.set_mode(light_mode::directional);
        lt.set_power(0.6f);
        world_.entity("light").set<transform_component>(light_trans).set<light_component>(lt);
    }

    void game_scene::init_player() {
        transform_component head_trans;
        head_trans.set_position({0.0f, 1.7f, 0.0f});

        camera_component cam;
        cam.skybox_->shader_ = asset_loader::instance().get_shader_program("skybox");
        cam.skybox_->texture_ = asset_loader::instance().get_texture_cube("skybox_sunset");

        auto body = world_.entity("head").add<transform_component>().add<body_tag>();
        auto head = world_.entity("body").set<transform_component>(head_trans).add<head_tag>().set<camera_component>(cam);
        head.child_of(body);
    }
} // mkr