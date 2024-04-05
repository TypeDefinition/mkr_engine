#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <log/log.h>
#include "graphics/renderer/graphics_renderer.h"
#include "graphics/shader/texture_unit.h"
#include "graphics/shader/forward_shader.h"
#include "graphics/shader/geometry_shader.h"
#include "graphics/shader/light_shader.h"
#include "graphics/shader/post_proc_shader.h"
#include "graphics/shader/skybox_shader.h"
#include "graphics/shader/shadow_2d_shader.h"
#include "graphics/shader/shadow_cubemap_shader.h"
#include "graphics/mesh/mesh_builder.h"
#include "graphics/shadow/shadow_bounds.h"

namespace mkr {
    void graphics_renderer::init() {
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
        app_window_ = std::make_unique<app_window>("mkr_engine", window_width_, window_height_, window_flags::none);

        // Initialise glew.
        if (GLEW_OK != glewInit()) {
            throw std::runtime_error("glewInit failed");
        }

        skybox_cube_ = mesh_builder::make_skybox("skybox");
        screen_quad_ = mesh_builder::make_screen_quad("screen_quad");

        // Framebuffers
        f_buff_ = std::make_unique<forward_buffer>(app_window_->width(), app_window_->height());
        for (auto i = 0; i < lighting::max_lights; ++i) {
            s2d_buff_[i] = std::make_unique<shadow_2d_buffer>(2048);
            scube_buff_[i] = std::make_unique<shadow_cubemap_buffer>(2048);
        }
    }

    void graphics_renderer::start() {
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);

        // Depth
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        // Blend
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Stencil
        // glEnable(GL_STENCIL_TEST);
        // glStencilMask(0xFF); // Each bit is written to the stencil buffer as-is.
        // glDisable(GL_STENCIL_TEST);
        // glStencilMask(0x00);
    }

    void graphics_renderer::update() {
        // Render
        render();

        // Swap buffer.
        app_window_->swap_buffers();
    }

    void graphics_renderer::exit() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void graphics_renderer::render() {
        // Shadow maps for spot and point lights can be shared between cameras.
        const auto num_lights = maths_util::min<int32_t>(lights_.size(), lighting::max_lights);
        for (auto i = 0; i < num_lights; ++i) {
            const auto& light = lights_[i].light_;
            const auto& trans = lights_[i].transform_;
            if (light.get_mode() == light_mode::point) {
                light_view_projection_matrix_[i] = point_shadow(scube_buff_[i].get(), trans, light);
            }
            else if (light.get_mode() == light_mode::spot) {
                light_view_projection_matrix_[i] = spot_shadow(s2d_buff_[i].get(), trans, light);
            }
        }

        // Render the scene once for every camera.
        while (!cameras_.empty()) {
            const auto& cam = cameras_.top().camera_;
            const auto& trans = cameras_.top().transform_;

            // Shadow maps for directional lights need to be recalculated once for each camera.
            for (auto i = 0; i < num_lights; ++i) {
                const auto& light = lights_[i].light_;
                if (light.get_mode() == light_mode::directional) {
                    light_view_projection_matrix_[i] = directional_shadow(s2d_buff_[i].get(), lights_[i].transform_, light, trans, cam);
                }
            }

            // In OpenGL convention, the camera looks down the -z axis.
            const auto& view_dir_x = -trans.left_;
            const auto& view_dir_y = trans.up_;
            const auto& view_dir_z = -trans.forward_;

            // View Matrix
            const auto view_matrix = matrix_util::view_matrix(trans.position_, trans.forward_, trans.up_);
            // Projection Matrix
            const auto projection_matrix = (cam.mode_ == projection_mode::perspective)
                                           ? matrix_util::perspective_matrix(cam.aspect_ratio_, cam.fov_, cam.near_plane_, cam.far_plane_)
                                           : matrix_util::orthographic_matrix(cam.aspect_ratio_, cam.ortho_size_, cam.near_plane_, cam.far_plane_);
            const auto inv_view_matrix = matrix_util::inverse_matrix(view_matrix).value_or(matrix4x4::identity());

            // Render forward pass.
            forward_pass(view_matrix, projection_matrix, inv_view_matrix, view_dir_z, view_dir_y, view_dir_x);

            // Pop camera off the priority queue.
            cameras_.pop();
        }

        // Clear objects for next frame.
        lights_.clear();
        deferred_meshes_.clear();
        forward_opaque_meshes_.clear();
        forward_transparent_meshes_.clear();
    }

    matrix4x4 graphics_renderer::point_shadow(shadow_cubemap_buffer* _buffer, const local_to_world& _trans, const light& _light) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, _buffer->width(), _buffer->height());

        _buffer->bind();
        _buffer->clear_depth_stencil();

        matrix4x4 projection_matrix = matrix_util::perspective_matrix(1.0f, maths_util::pi / 2.0f, 0.05f, 50.0f);
        matrix4x4 view_projection_matrices[6] = {
            projection_matrix * matrix_util::view_matrix(_trans.position_, vector3::left(), vector3::down()),
            projection_matrix * matrix_util::view_matrix(_trans.position_, vector3::right(), vector3::down()),
            projection_matrix * matrix_util::view_matrix(_trans.position_, vector3::up(), vector3::forwards()),
            projection_matrix * matrix_util::view_matrix(_trans.position_, vector3::down(), vector3::backwards()),
            projection_matrix * matrix_util::view_matrix(_trans.position_, vector3::forwards(), vector3::down()),
            projection_matrix * matrix_util::view_matrix(_trans.position_, vector3::backwards(), vector3::down()),
        };

        for (auto& material_iter : forward_opaque_meshes_) {
            auto shader = mkr::material::shadow_shader_cube_;
            shader->use();
            shader->set_uniform(shadow_cubemap_shader::uniform::u_view_projection_matrices0, false, view_projection_matrices[0]);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_view_projection_matrices1, false, view_projection_matrices[1]);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_view_projection_matrices2, false, view_projection_matrices[2]);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_view_projection_matrices3, false, view_projection_matrices[3]);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_view_projection_matrices4, false, view_projection_matrices[4]);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_view_projection_matrices5, false, view_projection_matrices[5]);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_light_pos, _trans.position_);
            shader->set_uniform(shadow_cubemap_shader::uniform::u_shadow_distance, _light.get_shadow_distance());

            // Draw to screen.
            for (auto& mesh_iter : material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& model_matrices = mesh_iter.second;

                std::vector<mesh_instance_data> batch;
                for (auto& model_matrix : model_matrices) {
                    batch.push_back({model_matrix, matrix3x3::identity()});
                }

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(batch);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, model_matrices.size());
            }
        }

        return matrix4x4::identity();
    }

    matrix4x4 graphics_renderer::spot_shadow(shadow_2d_buffer* _buffer, const local_to_world& _trans, const light& _light) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, _buffer->width(), _buffer->height()); // Viewport size MUST be buffer size, else it won't render to the buffer correctly.

        _buffer->bind();
        _buffer->clear_depth_stencil();

        const auto view_matrix = matrix_util::view_matrix(_trans.position_, _trans.forward_, _trans.up_);
        const auto projection_matrix = matrix_util::perspective_matrix(1.0f, _light.get_spotlight_outer_angle(), 0.1f, _light.get_shadow_distance());

        for (auto& material_iter : forward_opaque_meshes_) {
            auto shader = mkr::material::shadow_shader_2d_;
            shader->use();
            shader->set_uniform(shadow_2d_shader::uniform::u_view_matrix, false, view_matrix);
            shader->set_uniform(shadow_2d_shader::uniform::u_projection_matrix, false, projection_matrix);

            // Draw to screen.
            for (auto& mesh_iter : material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& model_matrices = mesh_iter.second;

                std::vector<mesh_instance_data> batch;
                for (auto& model_matrix : model_matrices) {
                    batch.push_back({model_matrix, matrix3x3::identity()});
                }

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(batch);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, model_matrices.size());
            }
        }

        return projection_matrix * view_matrix;
    }

    matrix4x4 graphics_renderer::directional_shadow(shadow_2d_buffer* _buffer, const local_to_world& _light_trans, const light& _light, const local_to_world& _cam_trans, const camera& _cam) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, _buffer->width(), _buffer->height()); // Viewport size MUST be buffer size, else it won't render to the buffer correctly.

        _buffer->bind();
        _buffer->clear_depth_stencil();

        const auto light_view_matrix = matrix_util::view_matrix(vector3::zero(), _light_trans.forward_, _light_trans.up_);
        const auto light_view_matrix_inverse = light_view_matrix.transposed(); // Since translation is 0, inverse equals transpose.
        const auto bounds = shadow_bounds::get_perspective_bounds(light_view_matrix,
                                                                  _cam_trans.position_,
                                                                  _cam_trans.left_, _cam_trans.up_, _cam_trans.forward_,
                                                                  _cam.near_plane_, _light.get_shadow_distance(),
                                                                  _cam.aspect_ratio_, _cam.fov_);
        const float width = bounds.max().x_ - bounds.min().x_;
        const float height = bounds.max().y_ - bounds.min().y_;
        const float near = bounds.min().z_ - bounds.centre().z_;
        const float far = bounds.max().z_ - bounds.centre().z_;
        const auto view_matrix = matrix_util::view_matrix(light_view_matrix_inverse * bounds.centre(), _light_trans.forward_, _light_trans.up_);
        const auto projection_matrix = matrix_util::orthographic_matrix(width / height, height, near, far);

        for (auto& material_iter : forward_opaque_meshes_) {
            auto shader = mkr::material::shadow_shader_2d_;
            shader->use();
            shader->set_uniform(shadow_2d_shader::uniform::u_view_matrix, false, view_matrix);
            shader->set_uniform(shadow_2d_shader::uniform::u_projection_matrix, false, projection_matrix);

            // Draw to screen.
            for (auto& mesh_iter : material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& model_matrices = mesh_iter.second;

                std::vector<mesh_instance_data> batch;
                for (auto& model_matrix : model_matrices) {
                    batch.push_back({model_matrix, matrix3x3::identity()});
                }

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(batch);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, model_matrices.size());
            }
        }

        return projection_matrix * view_matrix;
    }

    void graphics_renderer::forward_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_z, const vector3& _view_dir_y, const vector3& _view_dir_x) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, app_window_->width(), app_window_->height()); // Viewport size MUST be buffer size, else it won't render to the buffer correctly.

        framebuffer::bind_default_buffer();
        framebuffer::clear_default_buffer_colour(colour::blue());
        framebuffer::clear_default_depth_stencil();

        auto view_projection_matrix = _projection_matrix * _view_matrix;
        for (auto& material_iter : forward_opaque_meshes_) {
            auto material_ptr = material_iter.first;
            auto shader = material_ptr->forward_shader_;
            shader->use();

            // Bind textures.
            if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
            if (material_ptr->texture_gloss_) { material_ptr->texture_gloss_->bind(texture_unit::texture_gloss); }
            if (material_ptr->texture_displacement_) { material_ptr->texture_displacement_->bind(texture_unit::texture_displacement); }

            // Bind shadow maps.
            const auto num_lights = maths_util::min<int32_t>(lights_.size(), lighting::max_lights);
            for (auto i = 0; i < num_lights; ++i) {
                if (light_mode::point == lights_[i].light_.get_mode()) {
                    scube_buff_[i]->get_depth_stencil_attachment()->bind(texture_unit::cubemap_shadows0 + i);
                } else {
                    s2d_buff_[i]->get_depth_stencil_attachment()->bind(texture_unit::texture_shadows0 + i);
                }
            }

            // Transform
            shader->set_uniform(forward_shader::uniform::u_view_matrix, false, _view_matrix);
            shader->set_uniform(forward_shader::uniform::u_projection_matrix, false, _projection_matrix);
            shader->set_uniform(forward_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
            shader->set_uniform(forward_shader::uniform::u_texture_scale, material_ptr->texture_scale_);

            shader->set_uniform(forward_shader::uniform::u_inv_view_matrix, false, _inv_view_matrix);

            // Material
            shader->set_uniform(forward_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
            shader->set_uniform(forward_shader::uniform::u_specular_colour, material_ptr->specular_colour_);
            shader->set_uniform(forward_shader::uniform::u_gloss, material_ptr->gloss_);
            shader->set_uniform(forward_shader::uniform::u_displacement_scale, material_ptr->displacement_scale_);

            // Textures
            shader->set_uniform(forward_shader::uniform::u_texture_diffuse_enabled, material_ptr->texture_diffuse_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_normal_enabled, material_ptr->texture_normal_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_specular_enabled, material_ptr->texture_specular_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_gloss_enabled, material_ptr->texture_gloss_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_displacement_enabled, material_ptr->texture_displacement_ != nullptr);

            // Lights
            shader->set_uniform(forward_shader::uniform::u_num_lights, num_lights);
            shader->set_uniform(forward_shader::uniform::u_ambient_light, lighting::ambient_light_);

            for (auto i = 0; i < num_lights; ++i) {
                const auto& t = lights_[i].transform_;
                const auto& l = lights_[i].light_;

                const auto light_pos_view = _view_matrix * t.position_; // Light position in view space.
                const auto light_dir_view = vector3{_view_dir_x.dot(t.forward_), _view_dir_y.dot(t.forward_), _view_dir_z.dot(t.forward_)}.normalised(); // Light direction in view space.

                shader->set_uniform(i + forward_shader::uniform::u_light_mode0, l.get_mode());
                shader->set_uniform(i + forward_shader::uniform::u_light_power0, l.get_power());
                shader->set_uniform(i + forward_shader::uniform::u_light_colour0, l.get_colour());

                shader->set_uniform(i + forward_shader::uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
                shader->set_uniform(i + forward_shader::uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
                shader->set_uniform(i + forward_shader::uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());

                shader->set_uniform(i + forward_shader::uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                shader->set_uniform(i + forward_shader::uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());

                shader->set_uniform(i + forward_shader::uniform::u_light_position0, light_pos_view);
                shader->set_uniform(i + forward_shader::uniform::u_light_direction0, light_dir_view);

                shader->set_uniform(i + forward_shader::uniform::u_light_shadow_distance0, l.get_shadow_distance());
                shader->set_uniform(i + forward_shader::uniform::u_light_view_projection_matrix0, false, light_view_projection_matrix_[i]);
            }

            // Draw to screen.
            for (auto& mesh_iter : material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& model_matrices = mesh_iter.second;

                std::vector<mesh_instance_data> batch;
                for (auto& model_matrix : model_matrices) {
                    const auto model_view_inverse = matrix_util::inverse_matrix(_view_matrix * model_matrix).value_or(matrix4x4::identity());
                    const auto normal_matrix = matrix_util::minor_matrix(model_view_inverse.transposed(), 3, 3);
                    batch.push_back({model_matrix, normal_matrix});
                }

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(batch);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, model_matrices.size());
            }
        }
    }

    void graphics_renderer::submit_camera(const local_to_world& _transform, const camera& _camera) {
        cameras_.push({_transform, _camera});
    }

    void graphics_renderer::submit_light(const local_to_world& _transform, const light& _light) {
        lights_.push_back({_transform, _light});
    }

    void graphics_renderer::submit_mesh(const local_to_world& _transform, const render_mesh& _render_mesh) {
        if (_render_mesh.material_ == nullptr || _render_mesh.mesh_ == nullptr) { return; }
        switch (_render_mesh.material_->render_path_) {
            case render_path::deferred:
                // if (material::geometry_shader_ && material::light_shader_) {}
                deferred_meshes_[_render_mesh.material_][_render_mesh.mesh_].push_back(_transform.transform_);
                break;
            case render_path::forward_opaque:
                // if (_render_mesh.material_->forward_shader_) {}
                forward_opaque_meshes_[_render_mesh.material_][_render_mesh.mesh_].push_back(_transform.transform_);
                break;
            case render_path::forward_transparent:
                // if (_render_mesh.material_->forward_shader_) {}
                forward_transparent_meshes_[_render_mesh.material_][_render_mesh.mesh_].push_back(_transform.transform_);
                break;
            default:
                break;
        }
    }
} // mkr