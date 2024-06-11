#include "input/input_manager.h"
#include "graphics/mesh/mesh_manager.h"
#include "graphics/texture/texture_manager.h"
#include "graphics/material/material_manager.h"
#include "graphics/shader/shader_manager.h"
#include "ecs/component/renderable.h"
#include "ecs/component/light.h"
#include "ecs/component/camera.h"
#include "graphics/shader/forward_shader.h"
#include "graphics/shader/geometry_shader.h"
#include "graphics/shader/lighting_shader.h"
#include "graphics/shader/alpha_weight_shader.h"
#include "graphics/shader/alpha_blend_shader.h"
#include "graphics/shader/skybox_shader.h"
#include "graphics/shader/shadow_2d_shader.h"
#include "graphics/shader/shadow_cubemap_shader.h"
#include "game/scene/demo_scene.h"
#include "ecs/system/local_to_world_system.h"
#include "ecs/system/renderable_system.h"
#include "game/system/quit_system.h"
#include "game/system/player_movement.h"

namespace mkr {
    demo_scene::demo_scene() : scene("Demo Scene") {}

    demo_scene::~demo_scene() {}

    void demo_scene::init() {
        init_input();
        init_systems();
        init_shaders();
        init_meshes();
        init_materials();
        init_level();
    }

    void demo_scene::exit() {
        exit_input();
    }

    void demo_scene::init_input() {
        // input_manager::instance().set_relative_mouse(true);

        input_manager::instance().register_bool(quit, input_context_default, controller_index_default, keyboard_escape);

        input_manager::instance().register_bool(move_left, input_context_default, controller_index_default, keyboard_a);
        input_manager::instance().register_bool(move_right, input_context_default, controller_index_default, keyboard_d);
        input_manager::instance().register_bool(move_forward, input_context_default, controller_index_default, keyboard_w);
        input_manager::instance().register_bool(move_backward, input_context_default, controller_index_default, keyboard_s);

        input_manager::instance().register_axis1d(look_horizontal, input_context_default, controller_index_default, mouse_axis_x);
        input_manager::instance().register_axis1d(look_vertical, input_context_default, controller_index_default, mouse_axis_y);
    }

    void demo_scene::exit_input() {
        input_manager::instance().unregister_bool(quit, input_context_default, controller_index_default, keyboard_escape);

        input_manager::instance().unregister_bool(move_left, input_context_default, controller_index_default, keyboard_a);
        input_manager::instance().unregister_bool(move_right, input_context_default, controller_index_default, keyboard_d);
        input_manager::instance().unregister_bool(move_forward, input_context_default, controller_index_default, keyboard_w);
        input_manager::instance().unregister_bool(move_backward, input_context_default, controller_index_default, keyboard_s);

        input_manager::instance().unregister_axis1d(look_horizontal, input_context_default, controller_index_default, mouse_axis_x);
        input_manager::instance().unregister_axis1d(look_vertical, input_context_default, controller_index_default, mouse_axis_y);
    }

    void demo_scene::init_systems() {
        add_system<quit_system>();
        add_system<player_movement>();
        add_system<local_to_world_system>();
        add_system<renderable_system>();
    }

    void demo_scene::init_shaders() {
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

    void demo_scene::init_materials() {
        // Skybox
        /*texture_manager::instance().make_cubemap("skybox_test", {
            "./assets/textures/skyboxes/test/skybox_test_right.png",
            "./assets/textures/skyboxes/test/skybox_test_left.png",
            "./assets/textures/skyboxes/test/skybox_test_top.png",
            "./assets/textures/skyboxes/test/skybox_test_bottom.png",
            "./assets/textures/skyboxes/test/skybox_test_front.png",
            "./assets/textures/skyboxes/test/skybox_test_back.png",
        });*/

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

        /*
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
        */
    }

    void demo_scene::init_meshes() {
        mesh_manager::instance().make_mesh("sphere", "./assets/models/sphere.obj");
        mesh_manager::instance().make_mesh("cube", "./assets/models/cube.obj");
        mesh_manager::instance().make_mesh("plane", "./assets/models/plane.obj");
        mesh_manager::instance().make_mesh("quad", "./assets/models/quad.obj");
        mesh_manager::instance().make_mesh("monkey", "./assets/models/monkey.obj");
        mesh_manager::instance().make_mesh("cone", "./assets/models/cone.obj");
        mesh_manager::instance().make_mesh("torus", "./assets/models/torus.obj");
    }

    void demo_scene::init_level() {
        // Floor
        {
            auto ent = create_entity();
            auto transf = get_component<transform>(ent);
            get_component<transform>(ent)->set_scale({100.0f, 1.0f, 100.0f});
            auto rm = add_component<renderable>(ent);
            rm->mesh_ = mesh_manager::instance().get_mesh("plane");
            rm->material_ = material_manager::instance().get_material("tiles");
        }

        // Directional Light
        {
            quaternion rotation_x, rotation_y;
            rotation_x.set_rotation(vector3::x_axis(), 45.0f * maths_util::deg2rad);
            rotation_y.set_rotation(vector3::y_axis(), 45.0f * maths_util::deg2rad);

            auto ent = create_entity();
            get_component<transform>(ent)->set_rotation(rotation_x * rotation_y);

            auto lt = add_component<light>(ent);
            lt->set_shadow_distance(30.0f);
            lt->set_mode(light_mode::directional);
            lt->set_power(1.0f);
        }

        // Player
        {
            auto body = create_entity();
            add_component<player_body>(body);

            auto head = create_entity();
            add_component<player_head>(head);
            get_component<transform>(head)->set_position({0.0f, 1.7f, 0.0f});

            add_component<camera>(head)->skybox_.texture_ = texture_manager::instance().get_cubemap("skybox_sunset");
            get_component<camera>(head)->skybox_.shader_ = shader_manager::instance().get_shader("skybox");

            set_parent(head, body);
        }
    }

} // mkr