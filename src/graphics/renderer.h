#pragma once

#include <functional>
#include <common/singleton.h>
#include <maths/matrix_util.h>
#include "graphics/app_window.h"
#include "component/mesh_renderer.h"
#include "component/transform.h"
#include "component/camera.h"

namespace mkr {
    class renderer : public singleton<renderer> {
        friend class singleton<renderer>;

    private:
        std::unique_ptr<app_window> app_window_;

        matrix4x4 view_matrix_ = matrix4x4::identity();
        matrix4x4 projection_matrix_ = matrix4x4::identity();
        std::vector<matrix4x4> model_matrices_;
        std::vector<std::shared_ptr<mesh>> meshes_;
        std::vector<std::shared_ptr<texture_2d>> textures_;
        std::vector<std::shared_ptr<shader_program>> shaders_;

        matrix4x4 skybox_mvp_ = matrix4x4::identity();
        std::shared_ptr<mesh> skybox_mesh_;
        std::shared_ptr<texture_cube> skybox_texture_;
        std::shared_ptr<shader_program> skybox_shader_;

        renderer() {}

        virtual ~renderer() {}

        void draw_skybox();

        void draw_objects();

    public:
        void init();

        void start();

        void update();

        void exit();

        inline void set_skybox_texture(std::shared_ptr<texture_cube> _texture) { skybox_texture_ = _texture; }

        inline void set_skybox_shader(std::shared_ptr<shader_program> _shader) { skybox_shader_ = _shader; }

        std::function<void(transform&, camera&)> camera_system();

        std::function<void(transform&, mesh_renderer&)> mesh_system();
    };
}