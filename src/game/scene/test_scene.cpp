#include "test_scene.h"

namespace mkr {
    /*struct display {};

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

        input_manager::instance().register_button(debug_mode_off, input_context_default, controller_index_default, kc_0);
        input_manager::instance().register_button(debug_mode_position, input_context_default, controller_index_default, kc_1);
        input_manager::instance().register_button(debug_mode_normal, input_context_default, controller_index_default, kc_2);
        input_manager::instance().register_button(debug_mode_albedo, input_context_default, controller_index_default, kc_3);
        input_manager::instance().register_button(debug_mode_specular, input_context_default, controller_index_default, kc_4);
        input_manager::instance().register_button(debug_mode_gloss, input_context_default, controller_index_default, kc_5);
        input_manager::instance().register_button(debug_mode_material, input_context_default, controller_index_default, kc_6);
    }

    void test_scene::init_textures() {
        // Skybox
        asset_loader::instance().load_texture_cube("skybox_sunset", {
                "./../assets/textures/skyboxes/sunset/skybox_sunset_right.png",
                "./../assets/textures/skyboxes/sunset/skybox_sunset_left.png",
                "./../assets/textures/skyboxes/sunset/skybox_sunset_top.png",
                "./../assets/textures/skyboxes/sunset/skybox_sunset_bottom.png",
                "./../assets/textures/skyboxes/sunset/skybox_sunset_front.png",
                "./../assets/textures/skyboxes/sunset/skybox_sunset_back.png",
        });
    }

    void test_scene::init_meshes() {
        asset_loader::instance().load_obj("sphere", "./../assets/models/sphere.obj");
        asset_loader::instance().load_obj("cube", "./../assets/models/cube.obj");
        asset_loader::instance().load_obj("plane", "./../assets/models/plane.obj");
    }

    void test_scene::init_shaders() {
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

        debug_control_.gshader_ = asset_loader::instance().get_shader_program("gshader");
        debug_control_.lshader_ = asset_loader::instance().get_shader_program("lshader");
    }

    void test_scene::init_materials() {
        auto tiles = asset_loader::instance().make_material("tiles");
        tiles->texture_albedo_ = asset_loader::instance().load_texture_2d("tiles_001_albedo", "./../assets/textures/materials/tiles/tiles_001_albedo.png");
        tiles->texture_normal_ = asset_loader::instance().load_texture_2d("tiles_001_normal", "./../assets/textures/materials/tiles/tiles_001_normal.png");
        tiles->texture_displacement_ = asset_loader::instance().load_texture_2d("tiles_001_displacement", "./../assets/textures/materials/tiles/tiles_001_displacement.png");
        tiles->texture_gloss_ = asset_loader::instance().load_texture_2d("tiles_001_gloss", "./../assets/textures/materials/tiles/tiles_001_gloss.png");
        tiles->texture_specular_ = asset_loader::instance().load_texture_2d("tiles_001_specular", "./../assets/textures/materials/tiles/tiles_001_specular.png");
        tiles->texture_scale_ = vector2{1.0f, 1.0f} * 50.0f;
        tiles->displacement_scale_ = 0.1f;

        auto brick_wall = asset_loader::instance().make_material("brick_wall");
        brick_wall->texture_albedo_ = asset_loader::instance().load_texture_2d("brick_wall_001_albedo", "./../assets/textures/materials/brick_wall/brick_wall_001_albedo.png");
        brick_wall->texture_normal_ = asset_loader::instance().load_texture_2d("brick_wall_001_normal", "./../assets/textures/materials/brick_wall/brick_wall_001_normal.png");
        brick_wall->texture_displacement_ = asset_loader::instance().load_texture_2d("brick_wall_001_displacement", "./../assets/textures/materials/brick_wall/brick_wall_001_displacement.png");
        brick_wall->texture_gloss_ = asset_loader::instance().load_texture_2d("brick_wall_001_gloss", "./../assets/textures/materials/brick_wall/brick_wall_001_gloss.png");
        brick_wall->texture_specular_ = asset_loader::instance().load_texture_2d("brick_wall_001_specular", "./../assets/textures/materials/brick_wall/brick_wall_001_specular.png");
        brick_wall->texture_scale_ = vector2{1.0f, 1.0f} * 4.0f;
        brick_wall->displacement_scale_ = 0.05f;

        auto metal_chain_mail = asset_loader::instance().make_material("metal_chain_mail");
        metal_chain_mail->texture_albedo_ = asset_loader::instance().load_texture_2d("metal_chain_mail_001_albedo", "./../assets/textures/materials/metal/metal_chain_mail_001_albedo.png");
        metal_chain_mail->texture_normal_ = asset_loader::instance().load_texture_2d("metal_chain_mail_001_normal", "./../assets/textures/materials/metal/metal_chain_mail_001_normal.png");
        metal_chain_mail->texture_displacement_ = asset_loader::instance().load_texture_2d("metal_chain_mail_001_displacement", "./../assets/textures/materials/metal/metal_chain_mail_001_displacement.png");
        metal_chain_mail->texture_gloss_ = asset_loader::instance().load_texture_2d("metal_chain_mail_001_gloss", "./../assets/textures/materials/metal/metal_chain_mail_001_gloss.png");
        metal_chain_mail->texture_specular_ = asset_loader::instance().load_texture_2d("metal_chain_mail_001_specular", "./../assets/textures/materials/metal/metal_chain_mail_001_specular.png");
        metal_chain_mail->texture_scale_ = vector2{1.0f, 1.0f} * 8.0f;
        metal_chain_mail->displacement_scale_ = 0.05f;

        auto metal_pattern = asset_loader::instance().make_material("metal_pattern");
        metal_pattern->texture_albedo_ = asset_loader::instance().load_texture_2d("metal_pattern_001_albedo", "./../assets/textures/materials/metal/metal_pattern_001_albedo.png");
        metal_pattern->texture_normal_ = asset_loader::instance().load_texture_2d("metal_pattern_001_normal", "./../assets/textures/materials/metal/metal_pattern_001_normal.png");
        metal_pattern->texture_displacement_ = asset_loader::instance().load_texture_2d("metal_pattern_001_displacement", "./../assets/textures/materials/metal/metal_pattern_001_displacement.png");
        metal_pattern->texture_gloss_ = asset_loader::instance().load_texture_2d("metal_pattern_001_gloss", "./../assets/textures/materials/metal/metal_pattern_001_gloss.png");
        metal_pattern->texture_specular_ = asset_loader::instance().load_texture_2d("metal_pattern_001_specular", "./../assets/textures/materials/metal/metal_pattern_001_specular.png");
        metal_pattern->texture_scale_ = vector2{1.0f, 1.0f} * 2.0f;
        metal_pattern->displacement_scale_ = 0.1f;

        auto metal_plate = asset_loader::instance().make_material("metal_plate");
        metal_plate->texture_albedo_ = asset_loader::instance().load_texture_2d("metal_plate_001_albedo", "./../assets/textures/materials/metal/metal_plate_001_albedo.png");
        metal_plate->texture_normal_ = asset_loader::instance().load_texture_2d("metal_plate_001_normal", "./../assets/textures/materials/metal/metal_plate_001_normal.png");
        metal_plate->texture_displacement_ = asset_loader::instance().load_texture_2d("metal_plate_001_displacement", "./../assets/textures/materials/metal/metal_plate_001_displacement.png");
        metal_plate->texture_gloss_ = asset_loader::instance().load_texture_2d("metal_plate_001_gloss", "./../assets/textures/materials/metal/metal_plate_001_gloss.png");
        metal_plate->texture_specular_ = asset_loader::instance().load_texture_2d("metal_plate_001_specular", "./../assets/textures/materials/metal/metal_plate_001_specular.png");
        metal_plate->texture_scale_ = vector2{1.0f, 1.0f} * 12.0f;
        metal_plate->displacement_scale_ = 0.04f;

        auto rough_rock = asset_loader::instance().make_material("rough_rock");
        rough_rock->texture_albedo_ = asset_loader::instance().load_texture_2d("rough_rock_004_albedo", "./../assets/textures/materials/rough_rock/rough_rock_004_albedo.png");
        rough_rock->texture_normal_ = asset_loader::instance().load_texture_2d("rough_rock_004_normal", "./../assets/textures/materials/rough_rock/rough_rock_004_normal.png");
        rough_rock->texture_displacement_ = asset_loader::instance().load_texture_2d("rough_rock_004_displacement", "./../assets/textures/materials/rough_rock/rough_rock_004_displacement.png");
        rough_rock->texture_gloss_ = asset_loader::instance().load_texture_2d("rough_rock_004_gloss", "./../assets/textures/materials/rough_rock/rough_rock_004_gloss.png");
        rough_rock->texture_specular_ = asset_loader::instance().load_texture_2d("rough_rock_004_specular", "./../assets/textures/materials/rough_rock/rough_rock_004_specular.png");
        rough_rock->texture_scale_ = vector2{1.0f, 1.0f} * 2.0f;
        rough_rock->displacement_scale_ = 0.02f;

        auto rubble = asset_loader::instance().make_material("rubble");
        rubble->texture_albedo_ = asset_loader::instance().load_texture_2d("rubble_003_albedo", "./../assets/textures/materials/rubble/rubble_003_albedo.png");
        rubble->texture_normal_ = asset_loader::instance().load_texture_2d("rubble_003_normal", "./../assets/textures/materials/rubble/rubble_003_normal.png");
        rubble->texture_displacement_ = asset_loader::instance().load_texture_2d("rubble_003_displacement", "./../assets/textures/materials/rubble/rubble_003_displacement.png");
        rubble->texture_gloss_ = asset_loader::instance().load_texture_2d("rubble_003_gloss", "./../assets/textures/materials/rubble/rubble_003_gloss.png");
        rubble->texture_specular_ = asset_loader::instance().load_texture_2d("rubble_003_specular", "./../assets/textures/materials/rubble/rubble_003_specular.png");
        rubble->texture_scale_ = vector2{1.0f, 1.0f} * 4.0f;
        rubble->displacement_scale_ = 0.03f;

        auto pavement_brick = asset_loader::instance().make_material("pavement_brick");
        pavement_brick->texture_albedo_ = asset_loader::instance().load_texture_2d("pavement_brick_001_albedo", "./../assets/textures/materials/pavement/pavement_brick_001_albedo.png");
        pavement_brick->texture_normal_ = asset_loader::instance().load_texture_2d("pavement_brick_001_normal", "./../assets/textures/materials/pavement/pavement_brick_001_normal.png");
        pavement_brick->texture_displacement_ = asset_loader::instance().load_texture_2d("pavement_brick_001_displacement", "./../assets/textures/materials/pavement/pavement_brick_001_displacement.png");
        pavement_brick->texture_gloss_ = asset_loader::instance().load_texture_2d("pavement_brick_001_gloss", "./../assets/textures/materials/pavement/pavement_brick_001_gloss.png");
        pavement_brick->texture_specular_ = asset_loader::instance().load_texture_2d("pavement_brick_001_specular", "./../assets/textures/materials/pavement/pavement_brick_001_specular.png");
        pavement_brick->texture_scale_ = vector2{1.0f, 1.0f} * 4.0f;
        pavement_brick->displacement_scale_ = 0.1f;

        auto wooden_weave = asset_loader::instance().make_material("wooden_weave");
        wooden_weave->texture_albedo_ = asset_loader::instance().load_texture_2d("wooden_weave_002_albedo", "./../assets/textures/materials/wood/wooden_weave_002_albedo.png");
        wooden_weave->texture_normal_ = asset_loader::instance().load_texture_2d("wooden_weave_002_normal", "./../assets/textures/materials/wood/wooden_weave_002_normal.png");
        wooden_weave->texture_displacement_ = asset_loader::instance().load_texture_2d("wooden_weave_002_displacement", "./../assets/textures/materials/wood/wooden_weave_002_displacement.png");
        wooden_weave->texture_gloss_ = asset_loader::instance().load_texture_2d("wooden_weave_002_gloss", "./../assets/textures/materials/wood/wooden_weave_002_gloss.png");
        wooden_weave->texture_specular_ = asset_loader::instance().load_texture_2d("wooden_weave_002_specular", "./../assets/textures/materials/wood/wooden_weave_002_specular.png");
        wooden_weave->texture_scale_ = vector2{1.0f, 1.0f} * 5.0f;
        wooden_weave->displacement_scale_ = 0.05f;

        debug_control_.materials_2_[0] = brick_wall;
        debug_control_.materials_2_[1] = metal_chain_mail;
        debug_control_.materials_2_[2] = metal_pattern;
        debug_control_.materials_2_[3] = metal_plate;
        debug_control_.materials_2_[4] = rough_rock;
        debug_control_.materials_2_[5] = rubble;
        debug_control_.materials_2_[6] = pavement_brick;
        debug_control_.materials_2_[7] = wooden_weave;
    }

    void test_scene::init_systems() {
        world_.system<transform, const player_head>().each([&](transform& _transform, const player_head& _player_head) { head_control_(_transform, _player_head); });
        world_.system<transform, const player_body>().each([&](transform& _transform, const player_body& _player_body) { body_control_(_transform, _player_body); });
        world_.system<transform, const display>().each([&](transform& _transform, const display& _display) { _transform.rotate(quaternion{vector3::y_axis, 10.0f * application::instance().delta_time() * maths_util::deg2rad}); });

        world_.system<const root>().each(calculate_transforms);
        world_.system<const local_to_world_component, const camera>().each([](const local_to_world_component& _global_transform, const camera& _camera) { renderer::instance().update_cameras(_global_transform, _camera); });
        world_.system<const local_to_world_component, const light>().each([](const local_to_world_component& _global_transform, const light& _light) { renderer::instance().update_lights(_global_transform, _light); });
        world_.system<const local_to_world_component, const mesh_render>().each([](const local_to_world_component& _global_transform, const mesh_render& _mesh_renderer) { renderer::instance().update_objects(_global_transform, _mesh_renderer); });
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

        input_manager::instance().unregister_button(debug_mode_off, input_context_default, controller_index_default, kc_0);
        input_manager::instance().unregister_button(debug_mode_position, input_context_default, controller_index_default, kc_1);
        input_manager::instance().unregister_button(debug_mode_normal, input_context_default, controller_index_default, kc_2);
        input_manager::instance().unregister_button(debug_mode_albedo, input_context_default, controller_index_default, kc_3);
        input_manager::instance().unregister_button(debug_mode_specular, input_context_default, controller_index_default, kc_4);
        input_manager::instance().unregister_button(debug_mode_gloss, input_context_default, controller_index_default, kc_5);
        input_manager::instance().unregister_button(debug_mode_material, input_context_default, controller_index_default, kc_6);
    }

    void test_scene::init() {
        init_controls();
        init_meshes();
        init_textures();
        init_shaders();
        init_materials();
        init_systems();

        // Scene Root
        auto scene_root = world_.entity().add<root>().add<transform>();

        // Camera
        {
            transform head_trans;
            head_trans.set_position({0.0f, 1.7f, 0.0f});

            camera cam;
            cam.skybox_->shader_ = asset_loader::instance().get_shader_program("skybox");
            cam.skybox_->texture_ = asset_loader::instance().get_texture_cube("skybox_sunset");

            float angle = (360.0f / 8.0f) * (float)0;
            float x = std::sin(angle * maths_util::deg2rad);
            float z = std::cos(angle * maths_util::deg2rad);

            transform body_trans;
            body_trans.set_rotation(quaternion{vector3::y_axis, angle * maths_util::deg2rad});
            body_trans.set_position(11.0f * vector3{float(x), 0.0f, float(z)});

            auto body = world_.entity().set<transform>(body_trans).add<player_body>();
            auto head = world_.entity().set<transform>(head_trans).add<player_head>().set<camera>(cam);
            body.child_of(scene_root);
            head.child_of(body);
        }

        // Floor
        {
            transform trans;
            trans.set_scale({100.0f, 1.0f, 100.0f});
            trans.set_position({0.0f, 0.0f, 0.0f});

            mesh_render rend;
            rend.mesh_ = "plane";
            rend.material_ = "tiles";

            world_.entity().set<transform>(trans).set<mesh_render>(rend).child_of(scene_root);
        }

        // Spheres
        flecs::entity spheres[8];
        for (auto i = 0; i < 8; ++i) {
            float angle = (360.0f / 8.0f) * (float)i;
            float sphere_radius = 15.0f;
            float x = std::sin(angle * maths_util::deg2rad);
            float z = std::cos(angle * maths_util::deg2rad);

            transform sphere_trans;
            sphere_trans.set_position({x * sphere_radius, 1.5f, z * sphere_radius});
            sphere_trans.set_scale(3.0f * vector3{1.0f, 1.0f, 1.0f});
            mesh_render sphere_rend;
            sphere_rend.mesh_ = "sphere";
            spheres[i] = world_.entity().set<transform>(sphere_trans).set<mesh_render>(sphere_rend).add<display>().child_of(scene_root);

            transform light_trans;
            light_trans.set_position({0.0f, 15.0f, 0.0f});
            light_trans.set_rotation(quaternion(vector3::y_axis, angle * maths_util::deg2rad) * quaternion(vector3::x_axis, 30.0f * maths_util::deg2rad));
            light lt;
            lt.set_mode(light_mode::spot);
            lt.set_power(200.0f);
            world_.entity().set<transform>(light_trans).set<light>(lt).child_of(scene_root);
        }

        spheres[0].get_mut<mesh_render>()->material_ = "brick_wall";
        spheres[1].get_mut<mesh_render>()->material_ = "metal_chain_mail";
        spheres[2].get_mut<mesh_render>()->material_ = "metal_pattern";
        spheres[3].get_mut<mesh_render>()->material_ = "metal_plate";
        spheres[4].get_mut<mesh_render>()->material_ = "rough_rock";
        spheres[5].get_mut<mesh_render>()->material_ = "rubble";
        spheres[6].get_mut<mesh_render>()->material_ = "pavement_brick";
        spheres[7].get_mut<mesh_render>()->material_ = "wooden_weave";

        // Lights
        {
            transform trans;
            trans.set_position({0.0f, 0.0f, -10.0f});

            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis, 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis, 45.0f * maths_util::deg2rad);
            trans.set_rotation(rotation_y * rotation_x);

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
    }*/
}