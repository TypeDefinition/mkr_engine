#pragma once

#include <functional>
#include <queue>
#include <flecs.h>
#include <common/singleton.h>
#include <maths/matrix_util.h>
#include "graphics/renderer/stencil.h"
#include "graphics/app_window.h"
#include "graphics/framebuffer/shadow_2d_buffer.h"
#include "graphics/framebuffer/shadow_cubemap_buffer.h"
#include "graphics/framebuffer/geometry_buffer.h"
#include "graphics/framebuffer/lighting_buffer.h"
#include "graphics/framebuffer/forward_buffer.h"
#include "graphics/framebuffer/alpha_buffer.h"
#include "graphics/framebuffer/post_buffer.h"
#include "graphics/lighting/lighting.h"
#include "graphics/material/material.h"
#include "graphics/mesh/mesh_instance_data.h"
#include "component/render_mesh.h"
#include "component/transform.h"
#include "component/local_to_world.h"
#include "component/camera.h"
#include "component/light.h"

namespace mkr {
    class graphics_renderer : public singleton<graphics_renderer> {
        friend class singleton<graphics_renderer>;

    private:
        struct camera_data {
            local_to_world transform_;
            camera camera_;
            inline bool operator<(const camera_data& _rhs) const { return camera_.depth_ < _rhs.camera_.depth_; }
        };

        struct light_data {
            local_to_world transform_;
            light light_;
        };

        // App Window
        std::unique_ptr<app_window> app_window_;
        uint32_t window_width_ = 1920;
        uint32_t window_height_ = 1080;

        // Framebuffers
        std::unique_ptr<shadow_2d_buffer> s2d_buff_[lighting::max_lights];
        std::unique_ptr<shadow_cubemap_buffer> scube_buff_[lighting::max_lights];

        std::unique_ptr<geometry_buffer> g_buff_;
        std::unique_ptr<lighting_buffer> l_buff_;

        std::unique_ptr<forward_buffer> f_buff_;

        std::unique_ptr<alpha_buffer> a_buff_;

        matrix4x4 light_view_projection_matrix_[lighting::max_lights];

        // Screen Meshes
        std::unique_ptr<mesh> screen_quad_;
        std::unique_ptr<mesh> skybox_cube_;

        // Camera
        std::priority_queue<camera_data> cameras_;

        // Lights
        std::vector<light_data> lights_;

        // Meshes
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>> deferred_meshes_;
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>> forward_meshes_;
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>> transparent_meshes_;

        graphics_renderer() {}
        virtual ~graphics_renderer() {}

        void render();

        matrix4x4 point_shadow(shadow_cubemap_buffer* _buffer, const local_to_world& _trans, const light& _light);
        matrix4x4 spot_shadow(shadow_2d_buffer* _buffer, const local_to_world& _trans, const light& _light);
        matrix4x4 directional_shadow(shadow_2d_buffer* _buffer, const local_to_world& _light_trans, const light& _light, const local_to_world& _cam_trans, const camera& _cam);

        void geometry_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix);
        void lighting_pass(const matrix4x4& _view_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_x, const vector3& _view_dir_y, const vector3& _view_dir_z);
        void forward_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_x, const vector3& _view_dir_y, const vector3& _view_dir_z);
        void alpha_weight_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const matrix4x4& _inv_view_matrix, const vector3& _view_dir_x, const vector3& _view_dir_y, const vector3& _view_dir_z);
        void alpha_blend_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix);
        void skybox_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const skybox* _skybox);

    public:
        void init();
        void start();
        void update();
        void exit();

        void submit_camera(const local_to_world& _transform, const camera& _camera);
        void submit_light(const local_to_world& _transform, const light& _light);
        void submit_mesh(const local_to_world& _transform, const render_mesh& _render_mesh);
    };
} // mkr