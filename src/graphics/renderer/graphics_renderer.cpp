#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <log/log.h>
#include "graphics/renderer/graphics_renderer.h"
#include "graphics/shader/texture_unit.h"
#include "graphics/shader/shadow_2d_shader.h"
#include "graphics/shader/shadow_cubemap_shader.h"
#include "graphics/shader/geometry_shader.h"
#include "graphics/shader/lighting_shader.h"
#include "graphics/shader/forward_shader.h"
#include "graphics/shader/alpha_weight_shader.h"
#include "graphics/shader/alpha_blend_shader.h"
#include "graphics/shader/skybox_shader.h"
#include "graphics/mesh/mesh_builder.h"
#include "graphics/shadow/shadow_bounds.h"

namespace mkr {
    void graphics_renderer::init() {
        // Create Window
        app_window_ = std::make_unique<app_window>("mkr_engine", window_width_, window_height_, window_flags::none);

        skybox_cube_ = mesh_builder::make_skybox("skybox");
        screen_quad_ = mesh_builder::make_screen_quad("screen_quad");

        // Framebuffers
        for (auto i = 0; i < lighting::max_lights; ++i) {
            s2d_buff_[i] = std::make_unique<shadow_2d_buffer>(4096);
            scube_buff_[i] = std::make_unique<shadow_cubemap_buffer>(2048);
        }

        g_buff_ = std::make_unique<geometry_buffer>(app_window_->width(), app_window_->height());
        l_buff_ = std::make_unique<lighting_buffer>(app_window_->width(), app_window_->height());
        f_buff_ = std::make_unique<forward_buffer>(app_window_->width(), app_window_->height());

        a_buff_ = std::make_unique<alpha_buffer>(app_window_->width(), app_window_->height());
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
            auto& cam = cameras_.top().camera_;
            auto& trans = cameras_.top().transform_;

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
            const auto inv_view_matrix = matrix_util::inverse_matrix(view_matrix).value_or(matrix4x4::identity());

            // Projection Matrix
            const auto projection_matrix = (cam.mode_ == projection_mode::perspective)
                                           ? matrix_util::perspective_matrix(cam.aspect_ratio_, cam.fov_, cam.near_plane_, cam.far_plane_)
                                           : matrix_util::orthographic_matrix(cam.aspect_ratio_, cam.ortho_size_, cam.near_plane_, cam.far_plane_);


            // Render passes.
            geometry_pass(view_matrix, projection_matrix);
            lighting_pass(view_matrix, inv_view_matrix, view_dir_x, view_dir_y, view_dir_z);
            forward_pass(view_matrix, projection_matrix, inv_view_matrix, view_dir_x, view_dir_y, view_dir_z);
            skybox_pass(matrix_util::view_matrix(vector3::zero(), trans.forward_, trans.up_), projection_matrix, &cam.skybox_);
            alpha_weight_pass(view_matrix, projection_matrix, inv_view_matrix, view_dir_x, view_dir_y, view_dir_z);
            alpha_blend_pass(view_matrix, projection_matrix);

            // Blit result to default framebuffer.
            framebuffer::bind_default_buffer();
            framebuffer::clear_default_buffer_colour();
            framebuffer::clear_default_depth_stencil();

            f_buff_->set_read_colour_attachment(forward_buffer::colour_attachments::colour);
            f_buff_->blit_to(nullptr, true, false, false, 0, 0, f_buff_->width(), f_buff_->height(), 0, 0, app_window_->width(), app_window_->height());

            // Pop camera off the priority queue.
            cameras_.pop();
        }

        // Clear objects for next frame.
        lights_.clear();
        deferred_meshes_.clear();
        forward_meshes_.clear();
        transparent_meshes_.clear();
    }

    matrix4x4 graphics_renderer::point_shadow(shadow_cubemap_buffer* _buffer, const local_to_world& _trans, const light& _light) {
        glDisable(GL_BLEND);
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

        const auto draw_func = [&](std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>>& _meshes, bool _is_transparent) -> void {
            for (auto& material_iter : _meshes) {
                auto material_ptr = material_iter.first;

                if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }

                shader->set_uniform(shadow_cubemap_shader::uniform::u_is_transparent, _is_transparent);
                shader->set_uniform(shadow_cubemap_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
                shader->set_uniform(shadow_cubemap_shader::uniform::u_texture_scale, material_ptr->texture_scale_);
                shader->set_uniform(shadow_cubemap_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
                shader->set_uniform(shadow_cubemap_shader::uniform::u_has_texture_diffuse, material_ptr->texture_diffuse_ != nullptr);

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
        };

        draw_func(deferred_meshes_, false);
        draw_func(forward_meshes_, false);
        draw_func(transparent_meshes_, true);

        return matrix4x4::identity();
    }

    matrix4x4 graphics_renderer::spot_shadow(shadow_2d_buffer* _buffer, const local_to_world& _trans, const light& _light) {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, _buffer->width(), _buffer->height()); // Viewport size MUST be buffer size, else it won't render to the buffer correctly.

        _buffer->bind();
        _buffer->clear_depth_stencil();

        const auto view_matrix = matrix_util::view_matrix(_trans.position_, _trans.forward_, _trans.up_);
        const auto projection_matrix = matrix_util::perspective_matrix(1.0f, _light.get_spotlight_outer_angle(), 0.1f, _light.get_shadow_distance());

        auto shader = mkr::material::shadow_shader_2d_;
        shader->use();
        shader->set_uniform(shadow_2d_shader::uniform::u_view_matrix, false, view_matrix);
        shader->set_uniform(shadow_2d_shader::uniform::u_projection_matrix, false, projection_matrix);

        const auto draw_func = [&](std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>>& _meshes, bool _is_transparent) -> void {
            for (auto& material_iter : _meshes) {
                auto material_ptr = material_iter.first;

                if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }

                shader->set_uniform(shadow_2d_shader::uniform::u_is_transparent, _is_transparent);
                shader->set_uniform(shadow_2d_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
                shader->set_uniform(shadow_2d_shader::uniform::u_texture_scale, material_ptr->texture_scale_);
                shader->set_uniform(shadow_2d_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
                shader->set_uniform(shadow_2d_shader::uniform::u_has_texture_diffuse, material_ptr->texture_diffuse_ != nullptr);

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
        };

        draw_func(deferred_meshes_, false);
        draw_func(forward_meshes_, false);
        draw_func(transparent_meshes_, true);

        return projection_matrix * view_matrix;
    }

    matrix4x4 graphics_renderer::directional_shadow(shadow_2d_buffer* _buffer, const local_to_world& _light_trans, const light& _light, const local_to_world& _cam_trans, const camera& _cam) {
        glDisable(GL_BLEND);
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

        auto shader = mkr::material::shadow_shader_2d_;
        shader->use();
        shader->set_uniform(shadow_2d_shader::uniform::u_view_matrix, false, view_matrix);
        shader->set_uniform(shadow_2d_shader::uniform::u_projection_matrix, false, projection_matrix);

        const auto draw_func = [&](std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>>& _meshes, bool _is_transparent) -> void {
            for (auto& material_iter : _meshes) {
                auto material_ptr = material_iter.first;

                if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }

                shader->set_uniform(shadow_2d_shader::uniform::u_is_transparent, _is_transparent);
                shader->set_uniform(shadow_2d_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
                shader->set_uniform(shadow_2d_shader::uniform::u_texture_scale, material_ptr->texture_scale_);
                shader->set_uniform(shadow_2d_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
                shader->set_uniform(shadow_2d_shader::uniform::u_has_texture_diffuse, material_ptr->texture_diffuse_ != nullptr);

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
        };

        draw_func(deferred_meshes_, false);
        draw_func(forward_meshes_, false);
        draw_func(transparent_meshes_, true);

        return projection_matrix * view_matrix;
    }

    void graphics_renderer::geometry_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix) {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, g_buff_->width(), g_buff_->height()); // Viewport size MUST be buffer size, else it won't render to the buffer correctly.

        g_buff_->bind();
        g_buff_->set_draw_colour_attachment_all();
        g_buff_->clear_colour_all();
        g_buff_->clear_depth_stencil();

        for (auto& material_iter : deferred_meshes_) {
            auto material_ptr = material_iter.first;
            auto shader = material::geometry_shader_;
            shader->use();

            // Bind textures.
            if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
            if (material_ptr->texture_displacement_) { material_ptr->texture_displacement_->bind(texture_unit::texture_displacement); }

            // Transform
            shader->set_uniform(geometry_shader::uniform::u_view_matrix, false, _view_matrix);
            shader->set_uniform(geometry_shader::uniform::u_projection_matrix, false, _projection_matrix);
            shader->set_uniform(geometry_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
            shader->set_uniform(geometry_shader::uniform::u_texture_scale, material_ptr->texture_scale_);

            // Material
            shader->set_uniform(geometry_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
            shader->set_uniform(geometry_shader::uniform::u_specular_colour, material_ptr->specular_colour_);
            shader->set_uniform(geometry_shader::uniform::u_displacement_scale, material_ptr->displacement_scale_);

            // Textures
            shader->set_uniform(geometry_shader::uniform::u_has_texture_diffuse, material_ptr->texture_diffuse_ != nullptr);
            shader->set_uniform(geometry_shader::uniform::u_has_texture_normal, material_ptr->texture_normal_ != nullptr);
            shader->set_uniform(geometry_shader::uniform::u_has_texture_specular, material_ptr->texture_specular_ != nullptr);
            shader->set_uniform(geometry_shader::uniform::u_has_texture_displacement, material_ptr->texture_displacement_ != nullptr);

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

    void graphics_renderer::lighting_pass(const matrix4x4& _view_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_x, const vector3& _view_dir_y, const vector3& _view_dir_z) {
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, l_buff_->width(), l_buff_->height());

        l_buff_->bind();
        l_buff_->set_draw_colour_attachment_all();
        l_buff_->clear_colour_all();

        // Use shader.
        auto shader = material::light_shader_;
        shader->use();

        // Bind mesh.
        screen_quad_->bind();
        screen_quad_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});

        // Bind textures.
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::position)->bind(texture_unit::texture_position);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::normal)->bind(texture_unit::texture_normal);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::diffuse)->bind(texture_unit::texture_diffuse);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::specular)->bind(texture_unit::texture_specular);

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
        shader->set_uniform(lighting_shader::uniform::u_inv_view_matrix, false, _inv_view_matrix);

        // Lights.
        shader->set_uniform(lighting_shader::uniform::u_num_lights, num_lights);
        shader->set_uniform(lighting_shader::uniform::u_ambient_light, lighting::ambient_light_);

        for (auto i = 0; i < num_lights; ++i) {
            const auto& t = lights_[i].transform_;
            const auto& l = lights_[i].light_;

            const auto light_pos_view = _view_matrix * t.position_; // Light position in view space.
            const auto light_dir_view = vector3{_view_dir_x.dot(t.forward_), _view_dir_y.dot(t.forward_), _view_dir_z.dot(t.forward_)}.normalised(); // Light direction in view space.

            shader->set_uniform(i + lighting_shader::uniform::u_light_mode0, l.get_mode());
            shader->set_uniform(i + lighting_shader::uniform::u_light_power0, l.get_power());
            shader->set_uniform(i + lighting_shader::uniform::u_light_colour0, l.get_colour());

            shader->set_uniform(i + lighting_shader::uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
            shader->set_uniform(i + lighting_shader::uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
            shader->set_uniform(i + lighting_shader::uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());

            shader->set_uniform(i + lighting_shader::uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
            shader->set_uniform(i + lighting_shader::uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());

            shader->set_uniform(i + lighting_shader::uniform::u_light_position0, light_pos_view);
            shader->set_uniform(i + lighting_shader::uniform::u_light_direction0, light_dir_view);

            shader->set_uniform(i + lighting_shader::uniform::u_light_shadow_distance0, l.get_shadow_distance());
            shader->set_uniform(i + lighting_shader::uniform::u_light_view_projection_matrix0, false, light_view_projection_matrix_[i]);
        }

        // Draw.
        glDrawElementsInstanced(GL_TRIANGLES, screen_quad_->num_indices(), GL_UNSIGNED_INT, 0, 1);
    }

    void graphics_renderer::forward_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_x, const vector3& _view_dir_y, const vector3& _view_dir_z) {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glViewport(0, 0, f_buff_->width(), f_buff_->height());

        // Clear colour attachments.
        f_buff_->bind();
        f_buff_->set_draw_colour_attachment_all();
        f_buff_->clear_colour_all();
        f_buff_->clear_depth_stencil();

        // Blit colour.
        l_buff_->set_read_colour_attachment(lighting_buffer::colour_attachments::colour);
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::colour);
        l_buff_->blit_to(f_buff_.get(), true, false, false, 0, 0, l_buff_->width(), l_buff_->height(), 0, 0, f_buff_->width(), f_buff_->height());

        // Blit positions.
        g_buff_->set_read_colour_attachment(geometry_buffer::colour_attachments::position);
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::position);
        g_buff_->blit_to(f_buff_.get(), true, false, false, 0, 0, g_buff_->width(), g_buff_->height(), 0, 0, f_buff_->width(), f_buff_->height());

        // Blit normals.
        g_buff_->set_read_colour_attachment(geometry_buffer::colour_attachments::normal);
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::normal);
        g_buff_->blit_to(f_buff_.get(), true, false, false, 0, 0, g_buff_->width(), g_buff_->height(), 0, 0, f_buff_->width(), f_buff_->height());

        // Blit depth-stencil.
        g_buff_->blit_to(f_buff_.get(), false, true, true, 0, 0, g_buff_->width(), g_buff_->height(), 0, 0, f_buff_->width(), f_buff_->height());

        // Set draw attachments.
        f_buff_->set_draw_colour_attachment_all();

        for (auto& material_iter : forward_meshes_) {
            auto material_ptr = material_iter.first;
            auto shader = material_ptr->forward_shader_;
            shader->use();

            // Bind textures.
            if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
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
            shader->set_uniform(forward_shader::uniform::u_displacement_scale, material_ptr->displacement_scale_);

            // Textures
            shader->set_uniform(forward_shader::uniform::u_has_texture_diffuse, material_ptr->texture_diffuse_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_has_texture_normal, material_ptr->texture_normal_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_has_texture_specular, material_ptr->texture_specular_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_has_texture_displacement, material_ptr->texture_displacement_ != nullptr);

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

    void graphics_renderer::alpha_weight_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_x, const vector3& _view_dir_y, const vector3& _view_dir_z) {
        glEnable(GL_BLEND);
        glBlendFunci(alpha_buffer::colour_attachments::accumulation, GL_ONE, GL_ONE); // Accumulation blend target.
        glBlendFunci(alpha_buffer::colour_attachments::revealage, GL_ZERO, GL_ONE_MINUS_SRC_COLOR); // Revealage blend target.
        glEnable(GL_DEPTH_TEST); // We want to depth test against opaque objects,
        glDepthMask(GL_FALSE); // but not transparent objects.
        glDepthFunc(GL_LESS);

        glViewport(0, 0, a_buff_->width(), a_buff_->height());

        a_buff_->bind();
        a_buff_->set_draw_colour_attachment_all();
        a_buff_->clear_colour(alpha_buffer::colour_attachments::accumulation, colour::black());
        a_buff_->clear_colour(alpha_buffer::colour_attachments::revealage, colour{1.0f, 0.0f, 0.0f, 0.0f});
        a_buff_->clear_depth_stencil();

        // Blit depth-stencil.
        f_buff_->blit_to(a_buff_.get(), false, true, true, 0, 0, f_buff_->width(), f_buff_->height(), 0, 0, a_buff_->width(), a_buff_->height());

        for (auto& material_iter : transparent_meshes_) {
            auto material_ptr = material_iter.first;
            auto shader = material_ptr->alpha_weight_shader_;
            shader->use();

            // Bind textures.
            if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
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
            shader->set_uniform(alpha_weight_shader::uniform::u_view_matrix, false, _view_matrix);
            shader->set_uniform(alpha_weight_shader::uniform::u_projection_matrix, false, _projection_matrix);
            shader->set_uniform(alpha_weight_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
            shader->set_uniform(alpha_weight_shader::uniform::u_texture_scale, material_ptr->texture_scale_);

            shader->set_uniform(alpha_weight_shader::uniform::u_inv_view_matrix, false, _inv_view_matrix);

            // Material
            shader->set_uniform(alpha_weight_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
            shader->set_uniform(alpha_weight_shader::uniform::u_specular_colour, material_ptr->specular_colour_);
            shader->set_uniform(alpha_weight_shader::uniform::u_displacement_scale, material_ptr->displacement_scale_);

            // Textures
            shader->set_uniform(alpha_weight_shader::uniform::u_has_texture_diffuse, material_ptr->texture_diffuse_ != nullptr);
            shader->set_uniform(alpha_weight_shader::uniform::u_has_texture_normal, material_ptr->texture_normal_ != nullptr);
            shader->set_uniform(alpha_weight_shader::uniform::u_has_texture_specular, material_ptr->texture_specular_ != nullptr);
            shader->set_uniform(alpha_weight_shader::uniform::u_has_texture_displacement, material_ptr->texture_displacement_ != nullptr);

            // Lights
            shader->set_uniform(alpha_weight_shader::uniform::u_num_lights, num_lights);
            shader->set_uniform(alpha_weight_shader::uniform::u_ambient_light, lighting::ambient_light_);

            for (auto i = 0; i < num_lights; ++i) {
                const auto& t = lights_[i].transform_;
                const auto& l = lights_[i].light_;

                const auto light_pos_view = _view_matrix * t.position_; // Light position in view space.
                const auto light_dir_view = vector3{_view_dir_x.dot(t.forward_), _view_dir_y.dot(t.forward_), _view_dir_z.dot(t.forward_)}.normalised(); // Light direction in view space.

                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_mode0, l.get_mode());
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_power0, l.get_power());
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_colour0, l.get_colour());

                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());

                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());

                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_position0, light_pos_view);
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_direction0, light_dir_view);

                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_shadow_distance0, l.get_shadow_distance());
                shader->set_uniform(i + alpha_weight_shader::uniform::u_light_view_projection_matrix0, false, light_view_projection_matrix_[i]);
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

    void graphics_renderer::alpha_blend_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glViewport(0, 0, f_buff_->width(), f_buff_->height());

        f_buff_->bind();
        f_buff_->set_draw_colour_attachment_all();

        for (auto& material_iter : transparent_meshes_) {
            auto material_ptr = material_iter.first;
            auto shader = material_ptr->alpha_blend_shader_;
            shader->use();

            // Bind textures.
            a_buff_->get_colour_attachment(alpha_buffer::colour_attachments::accumulation)->bind(texture_unit::texture_accumulation);
            a_buff_->get_colour_attachment(alpha_buffer::colour_attachments::revealage)->bind(texture_unit::texture_revealage);

            // Transform
            shader->set_uniform(alpha_blend_shader::uniform::u_view_matrix, false, _view_matrix);
            shader->set_uniform(alpha_blend_shader::uniform::u_projection_matrix, false, _projection_matrix);

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

    void graphics_renderer::skybox_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const skybox* _skybox) {
        if (!_skybox || !_skybox->shader_) { return; }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE); // Do not write to the depth buffer.
        glDepthFunc(GL_LEQUAL); // We'll set our depth to 1 in the fragment shader.
        glViewport(0, 0, f_buff_->width(), f_buff_->height());

        /* Even though the skybox shader only writes to one colour attachment, we have to disable writing to the other colour attachments, otherwise they will have some undefined values written to them.
           As long as a colour attachment to set to be drawn to it, some value will be written to it no matter what, even if the shader does not specify.
           For the case of my computer, it will cause the values in colour_attachments::colour to also be written to the other colour attachments.
           [https://stackoverflow.com/questions/39545966/opengl-3-0-framebuffer-outputting-to-attachments-without-me-specifying] */
        f_buff_->bind();
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::colour);

        auto shader = _skybox->shader_;
        shader->use();

        if (_skybox->texture_) { _skybox->texture_->bind(texture_unit::texture_skybox); }

        shader->set_uniform(skybox_shader::uniform::u_view_matrix, false, _view_matrix);
        shader->set_uniform(skybox_shader::uniform::u_projection_matrix, false, _projection_matrix);
        shader->set_uniform(skybox_shader::uniform::u_colour, _skybox->colour_);
        shader->set_uniform(skybox_shader::uniform::u_texture_skybox_enabled, _skybox->texture_ != nullptr);

        skybox_cube_->bind();
        skybox_cube_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});
        glDrawElementsInstanced(GL_TRIANGLES, skybox_cube_->num_indices(), GL_UNSIGNED_INT, 0, 1);
    }

    void graphics_renderer::submit_camera(const local_to_world& _transform, const camera& _camera) {
        cameras_.push({_transform, _camera});
    }

    void graphics_renderer::submit_light(const local_to_world& _transform, const light& _light) {
        lights_.push_back({_transform, _light});
    }

    void graphics_renderer::submit_mesh(const local_to_world& _transform, const renderable& _renderable) {
        if (_renderable.material_ == nullptr || _renderable.mesh_ == nullptr) { return; }
        switch (_renderable.material_->render_path_) {
            case render_path::deferred:
                deferred_meshes_[_renderable.material_][_renderable.mesh_].push_back(_transform.transform_);
                break;
            case render_path::forward:
                forward_meshes_[_renderable.material_][_renderable.mesh_].push_back(_transform.transform_);
                break;
            case render_path::transparent:
                transparent_meshes_[_renderable.material_][_renderable.mesh_].push_back(_transform.transform_);
                break;
            default:
                break;
        }
    }
} // mkr