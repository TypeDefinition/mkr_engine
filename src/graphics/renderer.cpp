#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <log/log.h>
#include <maths/colour.h>
#include "graphics/renderer.h"
#include "scene/scene_manager.h"

namespace mkr {
    void renderer::draw_skybox() {
        if (!skybox_texture_ || !skybox_shader_ || !skybox_mesh_) {
            return;
        }

        glDisable(GL_DEPTH_TEST);

        skybox_shader_->set_uniform(shader_uniform::u_mat_mvp, false, skybox_mvp_);
        skybox_shader_->use();
        skybox_mesh_->bind();
        skybox_texture_->bind(texture_unit::texture_skybox);
        glDrawElements(GL_TRIANGLES, skybox_mesh_->num_indices(), GL_UNSIGNED_INT, 0);
    }

    void renderer::draw_objects() {
        // Render Objects
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        auto view_projection_matrix = projection_matrix_ * view_matrix_;
        for (size_t i = 0; i < model_matrices_.size(); ++i) {
            shaders_[i]->set_uniform(shader_uniform::u_mat_mvp, false, view_projection_matrix * model_matrices_[i]);
            shaders_[i]->use();
            meshes_[i]->bind();
            textures_[i]->bind(texture_unit::texture_albedo);
            glDrawElements(GL_TRIANGLES, meshes_[i]->num_indices(), GL_UNSIGNED_INT, 0);
        }

        model_matrices_.clear();
        meshes_.clear();
        textures_.clear();
        shaders_.clear();
    }

    void renderer::init() {
        // Initialise SDL_image.
        int img_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP;
        if (img_flags != (IMG_Init(img_flags) & img_flags)) {
            const std::string err_msg = "IMG_Init failed";
            mkr::log::error(err_msg);
            throw std::runtime_error(err_msg);
        }

        // Initialise SDL video subsystem.
        if (0 != SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            const std::string err_msg = "SDL_INIT_VIDEO failed";
            mkr::log::error(err_msg);
            throw std::runtime_error(err_msg);
        }

        // Enable Multisampling
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

        // Set Double Buffering
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // Set OpenGL Version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        int majorVersion, minorVersion;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
        mkr::log::info("OpenGL Version: {}.{}", majorVersion, minorVersion);

        // Create Window
        app_window_ = std::make_unique<app_window>("mkr_engine", 1920, 1080, window_flags::none);

        // Initialise glew.
        if (GLEW_OK != glewInit()) {
            throw std::runtime_error("glewInit failed");
        }

        skybox_mesh_ = asset_loader::instance().make_skybox("skybox");
    }

    void renderer::start() {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glEnable(GL_STENCIL_TEST);
        // glStencilMask(0xFF); // Each bit is written to the stencil buffer as-is.
        glDisable(GL_STENCIL_TEST);
        glStencilMask(0x00);
    }

    void renderer::update() {
        // Clear framebuffer.
        glClearNamedFramebufferfv(0, GL_COLOR, 0, (GLfloat*) &colour::blue.r_);
        float depth = 1.0f;
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth);

        // Render Skybox
        draw_skybox();
        draw_objects();

        // Swap buffer.
        app_window_->swap_buffers();
    }

    void renderer::exit() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        IMG_Quit();
    }

    std::function<void(transform&, camera&)> renderer::camera_system() {
        return [&](transform& _transform, camera& _camera)-> void {
            if (_camera.mode_ == projection_mode::perspective) {
                projection_matrix_ = matrix_util::perspective_matrix(_camera.aspect_ratio_, _camera.fov_, _camera.near_plane_, _camera.far_plane_);
            } else {
                projection_matrix_ = matrix_util::orthographic_matrix(_camera.aspect_ratio_, _camera.ortho_size_, _camera.near_plane_, _camera.far_plane_);
            }

            auto pos = _transform.local_position_;
            auto forward = quaternion::rotate(vector3::z_axis, _transform.local_rotation_);
            auto up = quaternion::rotate(vector3::y_axis, _transform.local_rotation_);
            view_matrix_ = matrix_util::view_matrix(pos, forward, up);

            skybox_mvp_ = projection_matrix_ * matrix_util::view_matrix(vector3::zero, forward, up);
        };
    }

    std::function<void(transform&, mesh_renderer&)> renderer::mesh_system() {
        return [&](transform& _transform, mesh_renderer& _mesh_renderer) {
            matrix4x4 translation_matrix = matrix_util::translation_matrix(_transform.local_position_);
            matrix4x4 rotation_matrix = _transform.local_rotation_.to_rotation_matrix();
            matrix4x4 scale_matrix = matrix_util::scale_matrix(_transform.local_scale_);
            matrix4x4 model_mat = translation_matrix * rotation_matrix * scale_matrix;

            model_matrices_.push_back(model_mat);
            meshes_.push_back(_mesh_renderer.mesh_);
            textures_.push_back(_mesh_renderer.texture_2d_);
            shaders_.push_back(_mesh_renderer.shader_);
        };
    }
}