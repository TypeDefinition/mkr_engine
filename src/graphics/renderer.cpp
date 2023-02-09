#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <log/log.h>
#include <maths/colour.h>
#include "graphics/renderer.h"
#include "graphics/asset_loader.h"

namespace mkr {
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

        skybox_mesh_ = asset_loader::instance().make_skybox();
        screen_quad_ = asset_loader::instance().make_screen_quad();

        // Framebuffers
        dbuffer_ = std::make_shared<dbuffer>();
        gbuffer_ = std::make_shared<gbuffer>(app_window_->width(), app_window_->height());
        lbuffer_ = std::make_shared<lbuffer>(app_window_->width(), app_window_->height());
        fbuffer_ = std::make_shared<fbuffer>(app_window_->width(), app_window_->height());
        pbuffer_ = std::make_shared<pbuffer>(app_window_->width(), app_window_->height());
    }

    void renderer::start() {
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

    void renderer::update() {
        // Clear framebuffer.
        dbuffer_->clear_colour_all();
        dbuffer_->clear_depth_stencil();

        // Render
        render();

        // Swap buffer.
        app_window_->swap_buffers();
    }

    void renderer::exit() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        IMG_Quit();
    }

    void renderer::update_cameras(const global_transform& _global_transform, const camera& _camera) {
        cameras_.push({_global_transform, _camera});
    }

    void renderer::update_lights(const global_transform& _global_transform, const light& _light) {
        lights_.push_back({_global_transform, _light});
    }

    void renderer::update_objects(const global_transform& _global_transform, const mesh_renderer& _mesh_renderer) {
        switch (_mesh_renderer.material_->render_path_) {
            case render_path::deferred:
                if (material::gshader_ && material::lshader_) {
                    deferred_objs_[_mesh_renderer.material_][_mesh_renderer.mesh_].push_back({_global_transform.model_matrix_, matrix3x3::identity()});
                }
                break;
            case render_path::forward_opaque:
                if (_mesh_renderer.material_->fshader_) {
                    forward_opaque_objs_[_mesh_renderer.material_][_mesh_renderer.mesh_].push_back({_global_transform.model_matrix_, matrix3x3::identity()});
                }
                break;
            case render_path::forward_transparent:
                if (_mesh_renderer.material_->fshader_) {
                    forward_transparent_objs_[_mesh_renderer.material_][_mesh_renderer.mesh_].push_back({_global_transform.model_matrix_, matrix3x3::identity()});
                }
                break;
            default:
                break;
        }
    }

    void renderer::render() {
        while (!cameras_.empty()) {
            auto cam_data = cameras_.top();
            cameras_.pop();
            auto& trans = cam_data.transform_;
            auto& cam = cam_data.camera_;

            auto view_matrix = matrix_util::view_matrix(trans.position_, trans.forward_, trans.up_);
            auto view_forward = trans.forward_;
            auto view_up = trans.up_;
            auto view_right = -trans.left_;
            matrix4x4 projection_matrix;
            if (cam.mode_ == projection_mode::perspective) {
                projection_matrix = matrix_util::perspective_matrix(cam.aspect_ratio_, cam.fov_, cam.near_plane_, cam.far_plane_);
            } else {
                projection_matrix = matrix_util::orthographic_matrix(cam.aspect_ratio_, cam.ortho_size_, cam.near_plane_, cam.far_plane_);
            }

            gpass(view_matrix, projection_matrix);
            lpass(view_matrix, view_forward, view_up, view_right);
            fpass(view_matrix, projection_matrix, view_forward, view_up, view_right, cam.skybox_);
            ppass(cam.near_plane_, cam.far_plane_);

            // Blit to default framebuffer.
            pbuffer_->set_read_colour_attachment(pbuffer_composite);
            pbuffer_->blit_to(dbuffer_, true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);
            dbuffer_->bind();
        }

        lights_.clear();
        deferred_objs_.clear();
        forward_opaque_objs_.clear();
        forward_transparent_objs_.clear();
    }

    void renderer::gpass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix) {
        // IMPORTANT: Disable blending so that and fragment with 0 in its alpha channel does not get discarded.
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFFFFFFFF); // Each bit is written to the stencil buffer as-is.
        glStencilFunc(GL_ALWAYS, stencil_opaque_object, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        gbuffer_->bind();
        gbuffer_->set_draw_colour_attachment_all();
        gbuffer_->clear_colour_all();
        gbuffer_->clear_depth_stencil();

        // Use shader.
        material::gshader_->use();

        auto view_projection_matrix = _projection_matrix * _view_matrix;
        for (auto& material_iter: deferred_objs_) {
            auto material_ptr = material_iter.first;

            // Bind textures.
            if (material_ptr->texture_albedo_) { material_ptr->texture_albedo_->bind(texture_unit::texture_albedo); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
            if (material_ptr->texture_gloss_) { material_ptr->texture_gloss_->bind(texture_unit::texture_gloss); }
            if (material_ptr->texture_displacement_) { material_ptr->texture_displacement_->bind(texture_unit::texture_displacement); }

            // Vertex shader uniforms.
            material::gshader_->set_uniform(shader_uniform::u_view_matrix, false, _view_matrix);
            material::gshader_->set_uniform(shader_uniform::u_projection_matrix, false, _projection_matrix);
            material::gshader_->set_uniform(shader_uniform::u_view_projection_matrix, false, view_projection_matrix);
            material::gshader_->set_uniform(shader_uniform::u_texture_offset, material_ptr->texture_offset_);
            material::gshader_->set_uniform(shader_uniform::u_texture_scale, material_ptr->texture_scale_);

            // Fragment shader uniforms.
            material::gshader_->set_uniform(shader_uniform::u_texture_albedo_enabled, material_ptr->texture_albedo_ != nullptr);
            material::gshader_->set_uniform(shader_uniform::u_texture_normal_enabled, material_ptr->texture_normal_ != nullptr);
            material::gshader_->set_uniform(shader_uniform::u_texture_specular_enabled, material_ptr->texture_specular_ != nullptr);
            material::gshader_->set_uniform(shader_uniform::u_texture_gloss_enabled, material_ptr->texture_gloss_ != nullptr);
            material::gshader_->set_uniform(shader_uniform::u_texture_displacement_enabled, material_ptr->texture_displacement_ != nullptr);

            material::gshader_->set_uniform(shader_uniform::u_albedo_colour, material_ptr->albedo_colour_);
            material::gshader_->set_uniform(shader_uniform::u_gloss, material_ptr->gloss_);
            material::gshader_->set_uniform(shader_uniform::u_displacement_scale, material_ptr->displacement_scale_);

            // Draw to screen.
            for (auto& mesh_iter: material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& instances = mesh_iter.second;
                std::for_each(instances.begin(), instances.end(), [=](mesh_instance& _inst) {
                    const auto model_view_matrix = _view_matrix * _inst.model_matrix_;
                    const auto model_view_inverse = matrix_util::inverse_matrix(model_view_matrix).value_or(matrix4x4::identity());
                    _inst.normal_matrix_ = matrix_util::minor_matrix(matrix_util::transpose_matrix(model_view_inverse), 3, 3);
                });

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(instances);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, instances.size());
            }
        }
    }

    void renderer::lpass(const matrix4x4& _view_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right) {
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        lbuffer_->bind();
        lbuffer_->set_draw_colour_attachment_all();
        lbuffer_->clear_colour_all();

        gbuffer_->get_colour_attachment(gbuffer_position)->bind(texture_unit::texture_frag_position);
        gbuffer_->get_colour_attachment(gbuffer_normal)->bind(texture_unit::texture_frag_normal);
        gbuffer_->get_colour_attachment(gbuffer_albedo)->bind(texture_unit::texture_frag_albedo);
        gbuffer_->get_colour_attachment(gbuffer_specular)->bind(texture_unit::texture_frag_specular);
        gbuffer_->get_colour_attachment(gbuffer_gloss)->bind(texture_unit::texture_frag_gloss);

        // Use shader.
        material::lshader_->use();

        // Lights.
        material::lshader_->set_uniform(shader_uniform::u_ambient_colour, material::ambient_colour_);
        material::lshader_->set_uniform(shader_uniform::u_enable_lights, material::enable_lights_);

        // Bind mesh.
        screen_quad_->bind();
        screen_quad_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});

        // If there are more lights than we can render in a single lighting pass, split it into multiple passes.
        const auto num_pass = (lights_.size() / max_lights) + 1;
        for (size_t p = 0; p < num_pass; ++p) {
            // Swap the diffuse and specular buffers.
            lbuffer_->get_colour_attachment(lbuffer_diffuse)->bind(texture_unit::texture_light_diffuse);
            lbuffer_->get_colour_attachment(lbuffer_specular)->bind(texture_unit::texture_light_specular);
            lbuffer_->swap_buffers();

            size_t start = max_lights * p;
            size_t end = maths_util::min<size_t>(start + max_lights, lights_.size());
            material::lshader_->set_uniform(shader_uniform::u_num_lights, (int)(end - start));

            size_t curr = start; // Index in C++.
            size_t index = 0; // Index in shader.
            while (curr < end) {
                const auto& t = lights_[curr].transform_;
                const auto& l = lights_[curr].light_;

                auto position_matrix = _view_matrix * t.model_matrix_;
                auto position_camera_space = vector3{position_matrix[3][0], position_matrix[3][1], position_matrix[3][2]};
                vector3 direction_vector = vector3{_view_right.dot(t.forward_), _view_up.dot(t.forward_), _view_forward.dot(t.forward_)}.normalised();

                material::lshader_->set_uniform(index + shader_uniform::u_light_mode0, l.get_mode());
                material::lshader_->set_uniform(index + shader_uniform::u_light_power0, l.get_power());
                material::lshader_->set_uniform(index + shader_uniform::u_light_colour0, l.get_colour());
                material::lshader_->set_uniform(index + shader_uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
                material::lshader_->set_uniform(index + shader_uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
                material::lshader_->set_uniform(index + shader_uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());
                material::lshader_->set_uniform(index + shader_uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                material::lshader_->set_uniform(index + shader_uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());
                material::lshader_->set_uniform(index + shader_uniform::u_light_position_camera_space0, position_camera_space);
                material::lshader_->set_uniform(index + shader_uniform::u_light_direction_camera_space0, direction_vector);

                ++curr;
                ++index;
            }

            // Draw.
            glDrawElementsInstanced(GL_TRIANGLES, screen_quad_->num_indices(), GL_UNSIGNED_INT, 0, 1);
        }
    }

    void renderer::fpass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right, std::shared_ptr<skybox> _skybox) {
        // Render opaque objects.
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFFFFFFFF); // Each bit is written to the stencil buffer as-is.
        glStencilFunc(GL_ALWAYS, stencil_opaque_object, 0xFFFFFFFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        fbuffer_->bind();
        fbuffer_->set_draw_colour_attachment_all();
        fbuffer_->clear_colour_all();

        lbuffer_->set_read_colour_attachment(lbuffer_composite);
        fbuffer_->set_draw_colour_attachment(fbuffer_composite);
        lbuffer_->blit_to(fbuffer_, true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);

        gbuffer_->set_read_colour_attachment(gbuffer_position);
        fbuffer_->set_draw_colour_attachment(fbuffer_position);
        gbuffer_->blit_to(fbuffer_, true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);

        gbuffer_->set_read_colour_attachment(gbuffer_normal);
        fbuffer_->set_draw_colour_attachment(fbuffer_normal);
        gbuffer_->blit_to(fbuffer_, true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);

        gbuffer_->blit_to(fbuffer_, false, true, true, 0, 0, 1920, 1080, 0, 0, 1920, 1080);
        fbuffer_->set_draw_colour_attachment_all();

        auto view_projection_matrix = _projection_matrix * _view_matrix;
        for (auto& material_iter: forward_opaque_objs_) {
            auto material_ptr = material_iter.first;
            auto shader = material_ptr->fshader_;
            shader->use();

            // Bind textures.
            if (material_ptr->texture_albedo_) { material_ptr->texture_albedo_->bind(texture_unit::texture_albedo); }
            if (material_ptr->texture_normal_) { material_ptr->texture_normal_->bind(texture_unit::texture_normal); }
            if (material_ptr->texture_specular_) { material_ptr->texture_specular_->bind(texture_unit::texture_specular); }
            if (material_ptr->texture_gloss_) { material_ptr->texture_gloss_->bind(texture_unit::texture_gloss); }
            if (material_ptr->texture_displacement_) { material_ptr->texture_displacement_->bind(texture_unit::texture_displacement); }

            // Vertex shader uniforms.
            shader->set_uniform(shader_uniform::u_view_matrix, false, _view_matrix);
            shader->set_uniform(shader_uniform::u_projection_matrix, false, _projection_matrix);
            shader->set_uniform(shader_uniform::u_view_projection_matrix, false, view_projection_matrix);
            shader->set_uniform(shader_uniform::u_texture_offset, material_ptr->texture_offset_);
            shader->set_uniform(shader_uniform::u_texture_scale, material_ptr->texture_scale_);

            // Fragment shader uniforms.
            shader->set_uniform(shader_uniform::u_texture_albedo_enabled, material_ptr->texture_albedo_ != nullptr);
            shader->set_uniform(shader_uniform::u_texture_normal_enabled, material_ptr->texture_normal_ != nullptr);
            shader->set_uniform(shader_uniform::u_texture_specular_enabled, material_ptr->texture_specular_ != nullptr);
            shader->set_uniform(shader_uniform::u_texture_gloss_enabled, material_ptr->texture_gloss_ != nullptr);
            shader->set_uniform(shader_uniform::u_texture_displacement_enabled, material_ptr->texture_displacement_ != nullptr);

            shader->set_uniform(shader_uniform::u_ambient_colour, material_ptr->ambient_colour_);
            shader->set_uniform(shader_uniform::u_albedo_colour, material_ptr->albedo_colour_);
            shader->set_uniform(shader_uniform::u_gloss, material_ptr->gloss_);
            shader->set_uniform(shader_uniform::u_displacement_scale, material_ptr->displacement_scale_);

            const int32_t num_lights = maths_util::min<int32_t >(lights_.size(), max_lights);
            shader->set_uniform(shader_uniform::u_enable_lights, material::enable_lights_);
            shader->set_uniform(shader_uniform::u_num_lights, num_lights);
            for (auto i = 0; i < num_lights; ++i) {
                const auto& t = lights_[i].transform_;
                const auto& l = lights_[i].light_;

                auto position_matrix = _view_matrix * t.model_matrix_;
                auto position_camera_space = vector3{position_matrix[3][0], position_matrix[3][1], position_matrix[3][2]};
                vector3 direction_vector = vector3{_view_right.dot(t.forward_), _view_up.dot(t.forward_), _view_forward.dot(t.forward_)}.normalised();

                shader->set_uniform(i + shader_uniform::u_light_mode0, l.get_mode());
                shader->set_uniform(i + shader_uniform::u_light_power0, l.get_power());
                shader->set_uniform(i + shader_uniform::u_light_colour0, l.get_colour());
                shader->set_uniform(i + shader_uniform::u_light_attenuation_constant0, l.get_attenuation_constant());
                shader->set_uniform(i + shader_uniform::u_light_attenuation_linear0, l.get_attenuation_linear());
                shader->set_uniform(i + shader_uniform::u_light_attenuation_quadratic0, l.get_attenuation_quadratic());
                shader->set_uniform(i + shader_uniform::u_light_spotlight_inner_cosine0, l.get_spotlight_inner_consine());
                shader->set_uniform(i + shader_uniform::u_light_spotlight_outer_cosine0, l.get_spotlight_outer_consine());
                shader->set_uniform(i + shader_uniform::u_light_position_camera_space0, position_camera_space);
                shader->set_uniform(i + shader_uniform::u_light_direction_camera_space0, direction_vector);
            }

            // Draw to screen.
            for (auto& mesh_iter: material_iter.second) {
                auto mesh_ptr = mesh_iter.first;
                auto& instances = mesh_iter.second;
                std::for_each(instances.begin(), instances.end(), [=](mesh_instance& _inst) {
                    const auto model_view_matrix = _view_matrix * _inst.model_matrix_;
                    const auto model_view_inverse = matrix_util::inverse_matrix(model_view_matrix).value_or(matrix4x4::identity());
                    _inst.normal_matrix_ = matrix_util::minor_matrix(matrix_util::transpose_matrix(model_view_inverse), 3, 3);
                });

                mesh_ptr->bind();
                mesh_ptr->set_instance_data(instances);
                glDrawElementsInstanced(GL_TRIANGLES, mesh_ptr->num_indices(), GL_UNSIGNED_INT, 0, instances.size());
            }
        }

        // Render skybox.
        // Even though the skybox shader only writes to one colour attachment, we have to disable writing to the other colour attachments, otherwise they will have some undefined values written to them.
        // As long as a colour attachment to set to be drawn to it, some value will be written to it no matter what, even if the shader does not specify.
        // For the case of my computer, it will cause the values in fbuffer_composite to also be written to the other colour attachments.
        // [https://stackoverflow.com/questions/39545966/opengl-3-0-framebuffer-outputting-to-attachments-without-me-specifying]
        fbuffer_->set_draw_colour_attachment(fbuffer_composite);
        if (_skybox && _skybox->shader_) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_STENCIL_TEST);
            glStencilMask(0xFFFFFFFF); // Each bit is written to the stencil buffer as-is.
            glStencilFunc(GL_NOTEQUAL, stencil_opaque_object, 0xFFFFFFFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            if (_skybox->texture_) { _skybox->texture_->bind(texture_unit::texture_skybox); }
            _skybox->shader_->use();
            _skybox->shader_->set_uniform(shader_uniform::u_texture_skybox_enabled, _skybox->texture_ != nullptr);
            _skybox->shader_->set_uniform(shader_uniform::u_sky_colour, _skybox->colour_);
            _skybox->shader_->set_uniform(shader_uniform::u_view_projection_matrix, false, _projection_matrix * matrix_util::view_matrix(vector3::zero, _view_forward, _view_up));

            skybox_mesh_->bind();
            skybox_mesh_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});
            glDrawElementsInstanced(GL_TRIANGLES, skybox_mesh_->num_indices(), GL_UNSIGNED_INT, 0, 1);
        }

        // Render transparent objects.
        fbuffer_->set_draw_colour_attachment_all();
        glEnable(GL_BLEND);
    }

    void renderer::ppass(float _near, float _far) {
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        pbuffer_->bind();
        pbuffer_->set_draw_colour_attachment_all();
        pbuffer_->clear_colour_all();

        fbuffer_->set_read_colour_attachment(fbuffer_composite);
        pbuffer_->set_draw_colour_attachment(pbuffer_composite);
        fbuffer_->blit_to(pbuffer_, true, false, false, 0, 0, 1920, 1080, 0, 0, 1920, 1080);
        pbuffer_->set_draw_colour_attachment_all();

        // Bind textures.
        fbuffer_->get_colour_attachment(fbuffer_position)->bind(texture_unit::texture_frag_position);
        fbuffer_->get_colour_attachment(fbuffer_normal)->bind(texture_unit::texture_frag_normal);
        pbuffer_->get_colour_attachment(pbuffer_composite)->bind(texture_unit::texture_composite);
        fbuffer_->get_depth_stencil_attachment()->bind(texture_unit::texture_depth_stencil);

        // Bind mesh.
        screen_quad_->bind();
        screen_quad_->set_instance_data({{matrix4x4::identity(), matrix3x3::identity()}});

        // Use shader.
        for (auto shader : material::pshaders_) {
            pbuffer_->get_colour_attachment(pbuffer_composite)->bind(texture_unit::texture_composite);
            pbuffer_->swap_buffers();

            shader->use();
            shader->set_uniform(shader_uniform::u_near, _near);
            shader->set_uniform(shader_uniform::u_far, _far);
            shader->set_uniform(shader_uniform::u_bottom_left, vector2::zero);
            shader->set_uniform(shader_uniform::u_top_right, vector2{1920.0f, 1080.0f});
            glDrawElementsInstanced(GL_TRIANGLES, screen_quad_->num_indices(), GL_UNSIGNED_INT, 0, 1);
        }
    }
}