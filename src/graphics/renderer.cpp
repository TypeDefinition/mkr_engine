#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <log/log.h>
#include <maths/colour.h>
#include "graphics/renderer.h"
#include "scene/scene_manager.h"
#include "graphics/asset_loader.h"

namespace mkr {
    void renderer::draw_skybox() {
        if (!skybox_texture_ || !skybox_shader_ || !skybox_mesh_) {
            return;
        }

        glDisable(GL_DEPTH_TEST);

        skybox_shader_->set_uniform(shader_uniform::view_projection_matrix, false, skybox_view_projection_matrix);
        skybox_shader_->use();
        skybox_texture_->bind(texture_unit::texture_skybox);
        skybox_mesh_->bind();
        skybox_mesh_->set_instance_data({{matrix4x4 ::identity()}});

        glDrawElementsInstanced(GL_TRIANGLES, skybox_mesh_->num_indices(), GL_UNSIGNED_INT, 0, 1);
    }

    void renderer::draw_objects() {
        // Render Objects
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        auto view_projection_matrix = projection_matrix_ * view_matrix_;
        for (const auto& iter: instances_) {
            const auto* mesh_rend = iter.first;
            const std::vector<mesh_instance>& instances = iter.second;

            // Vertex Shader
            mesh_rend->shader_->set_uniform(shader_uniform::view_matrix, false, view_matrix_);
            mesh_rend->shader_->set_uniform(shader_uniform::projection_matrix, false, projection_matrix_);
            mesh_rend->shader_->set_uniform(shader_uniform::view_projection_matrix, false, view_projection_matrix);

            // Fragment Shader
            mesh_rend->shader_->set_uniform(shader_uniform::ambient_colour, ambient_colour_);
            mesh_rend->shader_->set_uniform(shader_uniform::num_lights, (int)lights_.size());

            for (auto i = 0; i < lights_.size(); ++i) {
                const auto& t = lights_[i].first;
                const auto& l = lights_[i].second;

                auto position_matrix = view_matrix_ * t.model_matrix_;
                auto position_camera_space = vector3{position_matrix[3][0], position_matrix[3][1], position_matrix[3][2]};
                vector3 direction_vector = vector3{cam_right.dot(t.forward_), cam_up.dot(t.forward_), cam_forward.dot(t.forward_)}.normalised();

                mesh_rend->shader_->set_uniform(i + shader_uniform::light_mode0, l.get_mode());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_power0, l.get_power());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_colour0, l.get_colour());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_attenuation_constant0, l.get_attenuation_constant());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_attenuation_linear0, l.get_attenuation_linear());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_attenuation_quadratic0, l.get_attenuation_quadratic());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_position_camera_space0, position_camera_space);
                mesh_rend->shader_->set_uniform(i + shader_uniform::light_direction_camera_space0, direction_vector);
            }

            mesh_rend->shader_->use();
            mesh_rend->texture_2d_->bind(texture_unit::texture_albedo);
            mesh_rend->mesh_->bind();
            mesh_rend->mesh_->set_instance_data(instances);

            glDrawElementsInstanced(GL_TRIANGLES, mesh_rend->mesh_->num_indices(), GL_UNSIGNED_INT, 0, instances.size());
        }

        lights_.clear();
        instances_.clear();
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
        // glDisable(GL_STENCIL_TEST);
        // glStencilMask(0x00);
    }

    void renderer::update() {
        // Clear framebuffer.
        glClearNamedFramebufferfv(0, GL_COLOR, 0, (GLfloat*)&colour::blue.r_);
        float depth = 1.0f;
        glClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth);

        // Draw
        draw_skybox();
        draw_objects();

        // Swap buffer.
        app_window_->swap_buffers();
    }

    void renderer::exit() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        IMG_Quit();
    }

    void renderer::prep_cameras(const global_transform& _global_transform, const camera& _camera) {
        if (_camera.mode_ == projection_mode::perspective) {
            projection_matrix_ = matrix_util::perspective_matrix(_camera.aspect_ratio_, _camera.fov_, _camera.near_plane_, _camera.far_plane_);
        } else {
            projection_matrix_ = matrix_util::orthographic_matrix(_camera.aspect_ratio_, _camera.ortho_size_, _camera.near_plane_, _camera.far_plane_);
        }

        view_matrix_ = matrix_util::view_matrix(_global_transform.position_, _global_transform.forward_, _global_transform.up_);
        skybox_view_projection_matrix = projection_matrix_ * matrix_util::view_matrix(vector3::zero, _global_transform.forward_, _global_transform.up_);

        cam_forward = _global_transform.forward_;
        cam_up = _global_transform.up_;
        cam_right = -_global_transform.left_;
    }

    void renderer::prep_lights(const global_transform& _global_transform, const light& _light) {
        lights_.emplace_back(_global_transform, _light);
    }

    void renderer::sort_meshes(const global_transform& _global_transform, const mesh_renderer& _mesh_renderer) {
        const auto model_view_matrix = view_matrix_ * _global_transform.model_matrix_;
        const auto normal_matrix = matrix_util::minor_matrix(matrix_util::transpose_matrix(matrix_util::inverse_matrix(model_view_matrix).value()), 3, 3);
        instances_[&_mesh_renderer].push_back({_global_transform.model_matrix_, normal_matrix});
    }
}