#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <log/log.h>
#include "graphics/renderer/graphics_renderer.h"
#include "graphics/lighting/lighting.h"
#include "graphics/shader/texture_unit.h"
#include "graphics/shader/forward_shader.h"
#include "graphics/shader/geometry_shader.h"
#include "graphics/shader/light_shader.h"
#include "graphics/shader/post_proc_shader.h"
#include "graphics/shader/skybox_shader.h"
#include "graphics/mesh/mesh_builder.h"

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

        skybox_mesh_ = mesh_builder::make_skybox("skybox");
        screen_quad_ = mesh_builder::make_screen_quad("screen_quad");

        // Framebuffers
        d_buff_ = std::make_unique<default_buffer>();
        g_buff_ = std::make_unique<geometry_buffer>(app_window_->width(), app_window_->height());
        l_buff_ = std::make_unique<light_buffer>(app_window_->width(), app_window_->height());
        f_buff_ = std::make_unique<forward_buffer>(app_window_->width(), app_window_->height());
        pp_buff_ = std::make_unique<post_proc_buffer>(app_window_->width(), app_window_->height());
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
        // Clear framebuffer.
        d_buff_->clear_colour_all();
        d_buff_->clear_depth_stencil();

        // Render
        render();

        // Swap buffer.
        app_window_->swap_buffers();
    }

    void graphics_renderer::exit() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    void graphics_renderer::render() {
        // Render the scene once for every camera.
        while (!cameras_.empty()) {
            auto& cam_data = cameras_.top();
            auto& trans = cam_data.transform_;
            auto& cam = cam_data.camera_;

            // View Matrix
            const auto view_matrix = matrix_util::view_matrix(trans.position_, trans.forward_, trans.up_);
            const auto& view_forward = trans.forward_;
            const auto& view_up = trans.up_;
            const auto& view_right = -trans.left_;

            // Projection Matrix
            const matrix4x4 projection_matrix = (cam.mode_ == projection_mode::perspective)
                                          ? matrix_util::perspective_matrix(cam.aspect_ratio_, cam.fov_, cam.near_plane_, cam.far_plane_)
                                          : matrix_util::orthographic_matrix(cam.aspect_ratio_, cam.ortho_size_, cam.near_plane_, cam.far_plane_);

            // Render Passes
            geometry_pass(view_matrix, projection_matrix);
            light_pass(view_matrix, view_forward, view_up, view_right);
            forward_pass(view_matrix, projection_matrix, view_forward, view_up, view_right, cam.skybox_);
            post_proc_pass(cam.near_plane_, cam.far_plane_);

            // Blit to default framebuffer.
            pp_buff_->set_read_colour_attachment(post_proc_buffer::colour_attachments::composite);
            pp_buff_->blit_to(d_buff_.get(), true, false, false, 0, 0, window_width_, window_height_, 0, 0, window_width_, window_height_);
            d_buff_->bind();

            // Pop camera off the priority queue.
            cameras_.pop();
        }

        // Clear objects for next frame.
        lights_.clear();
        deferred_meshes_.clear();
        forward_opaque_meshes_.clear();
        forward_transparent_meshes_.clear();
    }

    void graphics_renderer::geometry_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix) {
        // IMPORTANT: Disable blending so that and fragment with 0 in its alpha channel does not get discarded.
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFFFFFFFF); // Each bit is written to the stencil buffer as-is.
        glStencilFunc(GL_ALWAYS, 0x10, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        g_buff_->bind();
        g_buff_->set_draw_colour_attachment_all();
        g_buff_->clear_colour_all();
        g_buff_->clear_depth_stencil();

        // Use shader.
        material::geometry_shader_->use();

        auto view_projection_matrix = _projection_matrix * _view_matrix;
        for (auto& material_iter : deferred_meshes_) {
            auto material_ptr = material_iter.first;

            // Bind textures.
            if (material_ptr->texture_diffuse_) { material_ptr->texture_diffuse_->bind(texture_unit::texture_diffuse); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
            if (material_ptr->texture_gloss_) { material_ptr->texture_gloss_->bind(texture_unit::texture_gloss); }
            if (material_ptr->texture_displacement_) { material_ptr->texture_displacement_->bind(texture_unit::texture_displacement); }

            // Vertex shader uniforms.
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_view_matrix, false, _view_matrix);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_projection_matrix, false, _projection_matrix);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_view_projection_matrix, false, view_projection_matrix);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_scale, material_ptr->texture_scale_);

            // Fragment shader uniforms.
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_diffuse_enabled, material_ptr->texture_diffuse_ != nullptr);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_normal_enabled, material_ptr->texture_normal_ != nullptr);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_specular_enabled, material_ptr->texture_specular_ != nullptr);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_gloss_enabled, material_ptr->texture_gloss_ != nullptr);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_texture_displacement_enabled, material_ptr->texture_displacement_ != nullptr);

            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_gloss, material_ptr->gloss_);
            material::geometry_shader_->set_uniform(geometry_shader::uniform::u_displacement_scale, material_ptr->displacement_scale_);

            // Draw to screen.
            for (auto& mesh_iter : material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& model_matrices = mesh_iter.second;

                std::vector<mesh_instance_data> batch;
                for (auto& model_matrix : model_matrices) {
                    const auto model_view_matrix = _view_matrix * model_matrix;
                    const auto model_view_inverse = matrix_util::inverse_matrix(model_view_matrix).value_or(matrix4x4::identity());
                    const auto normal_matrix = matrix_util::minor_matrix(matrix_util::transpose_matrix(model_view_inverse), 3, 3);
                    batch.push_back({model_matrix, normal_matrix});
                }

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(batch);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, model_matrices.size());
            }
        }
    }

    void graphics_renderer::light_pass(const matrix4x4& _view_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right) {
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        l_buff_->bind();
        l_buff_->set_draw_colour_attachment_all();
        l_buff_->clear_colour_all();

        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::position)->bind(texture_unit::texture_frag_position);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::normal)->bind(texture_unit::texture_frag_normal);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::diffuse)->bind(texture_unit::texture_frag_diffuse);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::specular)->bind(texture_unit::texture_frag_specular);
        g_buff_->get_colour_attachment(geometry_buffer::colour_attachments::gloss)->bind(texture_unit::texture_frag_gloss);

        // Use shader.
        material::light_shader_->use();

        // Lights.
        material::light_shader_->set_uniform(light_shader::uniform::u_ambient_light, lighting::ambient_light_);

        // Bind mesh.
        screen_quad_->bind();
        screen_quad_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});

        // If there are more lights than we can render in a single lighting pass, split it into multiple passes.
        const auto num_pass = (lights_.size() / light_shader::max_lights) + 1;
        for (size_t p = 0; p < num_pass; ++p) {
            // Swap the diffuse and specular buffers.
            l_buff_->get_colour_attachment(light_buffer::colour_attachments::diffuse)->bind(texture_unit::texture_light_diffuse);
            l_buff_->get_colour_attachment(light_buffer::colour_attachments::specular)->bind(texture_unit::texture_light_specular);
            l_buff_->swap_buffers();

            size_t start = light_shader::max_lights * p;
            size_t end = maths_util::min<size_t>(start + light_shader::max_lights, lights_.size());
            material::light_shader_->set_uniform(light_shader::uniform::u_num_lights, (int) (end - start));

            size_t curr = start; // Index in C++.
            size_t index = 0; // Index in shader.
            while (curr < end) {
                const auto& t = lights_[curr].transform_;
                const auto& l = lights_[curr].light_;

                auto position_matrix = _view_matrix * t.transform_;
                auto position_camera_space = vector3{position_matrix[3][0], position_matrix[3][1], position_matrix[3][2]};
                vector3 direction_vector = vector3{_view_right.dot(t.forward_), _view_up.dot(t.forward_), _view_forward.dot(t.forward_)}.normalised();

                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_mode0, l.get_mode());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_power0, l.get_power());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_colour0, l.get_colour());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_position_camera_space0, position_camera_space);
                material::light_shader_->set_uniform(index + light_shader::uniform::u_light_direction_camera_space0, direction_vector);

                ++curr;
                ++index;
            }

            // Draw.
            glDrawElementsInstanced(GL_TRIANGLES, screen_quad_->num_indices(), GL_UNSIGNED_INT, 0, 1);
        }
    }

    void graphics_renderer::forward_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right, const skybox& _skybox) {
        // Render opaque objects.
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFFFFFFFF); // Each bit is written to the stencil buffer as-is.
        glStencilFunc(GL_ALWAYS, 0x10, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        f_buff_->bind();
        f_buff_->set_draw_colour_attachment_all();
        f_buff_->clear_colour_all();

        l_buff_->set_read_colour_attachment(light_buffer::colour_attachments::composite);
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::composite);
        l_buff_->blit_to(f_buff_.get(), true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);

        g_buff_->set_read_colour_attachment(geometry_buffer::colour_attachments::position);
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::position);
        g_buff_->blit_to(f_buff_.get(), true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);

        g_buff_->set_read_colour_attachment(geometry_buffer::colour_attachments::normal);
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::normal);
        g_buff_->blit_to(f_buff_.get(), true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);

        g_buff_->blit_to(f_buff_.get(), false, true, true, 0, 0, 1920, 1080, 0, 0, 1920, 1080);
        f_buff_->set_draw_colour_attachment_all();

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

            // Vertex shader uniforms.
            shader->set_uniform(forward_shader::uniform::u_view_matrix, false, _view_matrix);
            shader->set_uniform(forward_shader::uniform::u_projection_matrix, false, _projection_matrix);
            shader->set_uniform(forward_shader::uniform::u_view_projection_matrix, false, view_projection_matrix);
            shader->set_uniform(forward_shader::uniform::u_texture_offset, material_ptr->texture_offset_);
            shader->set_uniform(forward_shader::uniform::u_texture_scale, material_ptr->texture_scale_);

            // Fragment shader uniforms.
            shader->set_uniform(forward_shader::uniform::u_texture_diffuse_enabled, material_ptr->texture_diffuse_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_normal_enabled, material_ptr->texture_normal_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_specular_enabled, material_ptr->texture_specular_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_gloss_enabled, material_ptr->texture_gloss_ != nullptr);
            shader->set_uniform(forward_shader::uniform::u_texture_displacement_enabled, material_ptr->texture_displacement_ != nullptr);

            shader->set_uniform(forward_shader::uniform::u_ambient_light, lighting::ambient_light_);
            shader->set_uniform(forward_shader::uniform::u_diffuse_colour, material_ptr->diffuse_colour_);
            shader->set_uniform(forward_shader::uniform::u_gloss, material_ptr->gloss_);
            shader->set_uniform(forward_shader::uniform::u_displacement_scale, material_ptr->displacement_scale_);

            const int32_t num_lights = maths_util::min<int32_t>(lights_.size(), forward_shader::max_lights);
            shader->set_uniform(forward_shader::uniform::u_num_lights, num_lights);
            for (auto i = 0; i < num_lights; ++i) {
                const auto& t = lights_[i].transform_;
                const auto& l = lights_[i].light_;

                auto position_matrix = _view_matrix * t.transform_;
                auto position_camera_space = vector3{position_matrix[3][0], position_matrix[3][1], position_matrix[3][2]};
                vector3 direction_vector = vector3{_view_right.dot(t.forward_), _view_up.dot(t.forward_), _view_forward.dot(t.forward_)}.normalised();

                shader->set_uniform(i + forward_shader::uniform::u_light_mode0, l.get_mode());
                shader->set_uniform(i + forward_shader::uniform::u_light_power0, l.get_power());
                shader->set_uniform(i + forward_shader::uniform::u_light_colour0, l.get_colour());
                shader->set_uniform(i + forward_shader::uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
                shader->set_uniform(i + forward_shader::uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
                shader->set_uniform(i + forward_shader::uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());
                shader->set_uniform(i + forward_shader::uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                shader->set_uniform(i + forward_shader::uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());
                shader->set_uniform(i + forward_shader::uniform::u_light_position_camera_space0, position_camera_space);
                shader->set_uniform(i + forward_shader::uniform::u_light_direction_camera_space0, direction_vector);
            }

            // Draw to screen.
            for (auto& mesh_iter : material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& model_matrices = mesh_iter.second;

                std::vector<mesh_instance_data> batch;
                for (auto& model_matrix : model_matrices) {
                    const auto model_view_matrix = _view_matrix * model_matrix;
                    const auto model_view_inverse = matrix_util::inverse_matrix(model_view_matrix).value_or(matrix4x4::identity());
                    const auto normal_matrix = matrix_util::minor_matrix(matrix_util::transpose_matrix(model_view_inverse), 3, 3);
                    batch.push_back({model_matrix, normal_matrix});
                }

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(batch);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, model_matrices.size());
            }
        }

        // Render skybox.
        // Even though the skybox shader only writes to one colour attachment, we have to disable writing to the other colour attachments, otherwise they will have some undefined values written to them.
        // As long as a colour attachment to set to be drawn to it, some value will be written to it no matter what, even if the shader does not specify.
        // For the case of my computer, it will cause the values in fbuffer_composite to also be written to the other colour attachments.
        // [https://stackoverflow.com/questions/39545966/opengl-3-0-framebuffer-outputting-to-attachments-without-me-specifying]
        f_buff_->set_draw_colour_attachment(forward_buffer::colour_attachments::composite);
        if (_skybox.shader_) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_STENCIL_TEST);
            glStencilMask(0xFFFFFFFF); // Each bit is written to the stencil buffer as-is.
            glStencilFunc(GL_NOTEQUAL, 0x10, 0xFFFFFFFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            if (_skybox.texture_) { _skybox.texture_->bind(texture_unit::texture_skybox); }
            _skybox.shader_->use();
            _skybox.shader_->set_uniform(skybox_shader::uniform::u_texture_skybox_enabled, _skybox.texture_ != nullptr);
            _skybox.shader_->set_uniform(skybox_shader::uniform::u_skybox_colour, _skybox.colour_);
            _skybox.shader_->set_uniform(skybox_shader::uniform::u_view_projection_matrix, false, _projection_matrix * matrix_util::view_matrix(vector3::zero, _view_forward, _view_up));

            skybox_mesh_->bind();
            skybox_mesh_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});
            glDrawElementsInstanced(GL_TRIANGLES, skybox_mesh_->num_indices(), GL_UNSIGNED_INT, 0, 1);
        }

        // Render transparent objects.
        f_buff_->set_draw_colour_attachment_all();
        glEnable(GL_BLEND);
    }

    void graphics_renderer::post_proc_pass(float _near, float _far) {
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        pp_buff_->bind();
        pp_buff_->set_draw_colour_attachment_all();
        pp_buff_->clear_colour_all();

        f_buff_->set_read_colour_attachment(forward_buffer::colour_attachments::composite);
        pp_buff_->set_draw_colour_attachment(post_proc_buffer::colour_attachments::composite);
        f_buff_->blit_to(pp_buff_.get(), true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);
        pp_buff_->set_draw_colour_attachment_all();

        // Bind textures.
        f_buff_->get_colour_attachment(forward_buffer::colour_attachments::position)->bind(texture_unit::texture_frag_position);
        f_buff_->get_colour_attachment(forward_buffer::colour_attachments::normal)->bind(texture_unit::texture_frag_normal);
        pp_buff_->get_colour_attachment(post_proc_buffer::colour_attachments::composite)->bind(texture_unit::texture_composite);
        f_buff_->get_depth_stencil_attachment()->bind(texture_unit::texture_depth_stencil);

        // Bind mesh.
        screen_quad_->bind();
        screen_quad_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});

        // Use shader.
        for (auto shader : material::post_proc_shaders_) {
            pp_buff_->get_colour_attachment(post_proc_buffer::colour_attachments::composite)->bind(texture_unit::texture_composite);
            pp_buff_->swap_buffers();

            shader->use();
            shader->set_uniform(post_proc_shader::uniform::u_near, _near);
            shader->set_uniform(post_proc_shader::uniform::u_far, _far);
            shader->set_uniform(post_proc_shader::uniform::u_bottom_left, vector2::zero);
            shader->set_uniform(post_proc_shader::uniform::u_top_right, vector2{1920.0f, 1080.0f});
            glDrawElementsInstanced(GL_TRIANGLES, screen_quad_->num_indices(), GL_UNSIGNED_INT, 0, 1);
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