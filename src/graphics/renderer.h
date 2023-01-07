#pragma once

#include <GL/glew.h>
#include <spdlog/spdlog.h>
#include <maths/matrix_util.h>
#include "util/file_util.h"
#include "graphics/asset_loader.h"
#include "graphics/shader_program.h"
#include "input/input_system.h"
#include "component/camera.h"
#include "component/light.h"
#include "component/transform.h"
#include "application/application.h"

namespace mkr {
    class renderer {
    private:
        input_name input_quit = 0;
        input_name input_name_left = 1;
        input_name input_name_right = 2;

        std::shared_ptr<mesh> mesh_;
        std::shared_ptr<shader_program> sp_;
        std::shared_ptr<texture_2d> texture_2d_;
        transform transform_;
        float rotation_angle_ = 0.0f;

        event_listener input_listener_;

    public:
        renderer() {
            mesh_ = asset_loader::instance().make_quad("quad");
            auto vs_src = file_util::file_to_str("/mnt/ZorinWork/mkr_engine/src/shaders/vertex_shader.vs");
            auto fs_src = file_util::file_to_str("/mnt/ZorinWork/mkr_engine/src/shaders/fragment_shader.fs");
            std::vector<std::string> vs = {vs_src};
            std::vector<std::string> fs = {fs_src};
            sp_ = std::make_shared<shader_program>("sp", vs, fs);

            texture_2d_ = asset_loader::instance().load_texture_2d("texture_2d", "/mnt/ZorinWork/mkr_engine/textures/Test_0.png");

            transform_.local_position_ = {0.0f, 0.0f, 2.0f};
            transform_.local_rotation_ = quaternion{vector3::y_axis, maths_util::pi};

            input_system::instance().register_button(input_quit, input::get_input_mask(input_context_default, controller_index_default, kc_escape));
            input_system::instance().register_button(input_name_left, input::get_input_mask(input_context_default, controller_index_default, kc_left));
            input_system::instance().register_button(input_name_right, input::get_input_mask(input_context_default, controller_index_default, kc_right));

            input_listener_.set_callback([&](const event* _event) {
                const auto* e = dynamic_cast<const button_event*>(_event);
                if (!e) { return; }
                if (e->state_ != button_event::button_state::pressed) { return; }

                if (e->name_ == input_quit) {
                    application::instance().terminate();
                }

                if (e->name_ == input_name_left) {
                    rotation_angle_ -= 180.0f * application::instance().delta_time();
                }

                if (e->name_ == input_name_right) {
                    rotation_angle_ += 180.0f * application::instance().delta_time();
                }
            });
            input_system::instance().get_event_dispatcher()->add_listener<button_event>(&input_listener_);
        }

        ~renderer() {}

        void update() {
            glClearNamedFramebufferfv(0, GL_COLOR, 0, (GLfloat*)&colour::blue.r_);
            float depth = 1.0f;
            glClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth);

            glEnable(GL_MULTISAMPLE);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);

            // glEnable(GL_BLEND);
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // glEnable(GL_STENCIL_TEST);
            // glStencilMask(0xFF); // Each bit is written to the stencil buffer as-is.
            // glDisable(GL_STENCIL_TEST);
            // glStencilMask(0x00);

            matrix4x4 translation_matrix = matrix_util::translation_matrix(transform_.local_position_);
            // quaternion rotation_x{vector3::x_axis, 30.0f * application::instance().delta_time() * maths_util::deg2rad};
            // quaternion rotation_y{vector3::y_axis, 30.0f * application::instance().delta_time() * maths_util::deg2rad};
            // quaternion rotation_z{vector3::z_axis, 30.0f * application::instance().delta_time() * maths_util::deg2rad};
            // transform_.local_rotation_ = rotation_x * rotation_y * rotation_z * transform_.local_rotation_;
            // transform_.local_rotation_ = rotation_z * transform_.local_rotation_;
            quaternion rot{vector3::z_axis, rotation_angle_ * maths_util::deg2rad};
            transform_.local_rotation_ = rot * quaternion{vector3::y_axis, maths_util::pi};

            matrix4x4 rotation_matrix = transform_.local_rotation_.to_rotation_matrix();
            matrix4x4 scale_matrix = matrix_util::scale_matrix(transform_.local_scale_);

            auto model_mat = translation_matrix * rotation_matrix * scale_matrix;
            auto view_mat = matrix_util::view_matrix(vector3::zero, vector3::z_axis, vector3::y_axis);
            auto perspective_mat = matrix_util::perspective_matrix(16.0f / 9.0f, 70.0f * maths_util::deg2rad, 0.1f, 100.0f);
            auto ortho_mat = matrix_util::orthographic_matrix(16.0f / 9.0f, 2.0f, 0.1f, 100.0f);

            sp_->set_uniform(shader_uniform::u_mat_mvp, false, perspective_mat * view_mat * model_mat);
            // sp_->set_uniform(shader_uniform::u_mat_mvp, false, ortho_mat * view_mat * model_mat);

            sp_->use();
            mesh_->bind();
            texture_2d_->bind(texture_unit::texture_albedo);

            glDrawElements(GL_TRIANGLES, mesh_->num_indices(), GL_UNSIGNED_INT, 0);
        }
    };
}