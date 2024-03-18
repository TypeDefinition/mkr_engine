#pragma once

#include <functional>
#include <queue>
#include <flecs.h>
#include <common/singleton.h>
#include <maths/matrix_util.h>
#include "graphics/app_window.h"
#include "graphics/framebuffer/default_buffer.h"
#include "graphics/framebuffer/geometry_buffer.h"
#include "graphics/framebuffer/light_buffer.h"
#include "graphics/framebuffer/post_proc_buffer.h"
#include "graphics/framebuffer/forward_buffer.h"
#include "graphics/material/material.h"
#include "graphics/mesh/mesh_instance_data.h"
#include "component/render_mesh.h"
#include "component/transform.h"
#include "component/local_to_world.h"
#include "component/camera.h"
#include "component/light.h"

namespace mkr {
    // There are 8 bits in the stencil buffer.
    enum stencil_value : uint8_t {
        placeholder0 = 1,
        placeholder1 = 2,
        placeholder2 = 4,
        placeholder3 = 8,
        placeholder4 = 16,
        placeholder5 = 32,
        placeholder6 = 64,
        placeholder7 = 128,
    };

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
        std::unique_ptr<default_buffer> d_buff_;
        std::unique_ptr<geometry_buffer> g_buff_;
        std::unique_ptr<light_buffer> l_buff_;
        std::unique_ptr<forward_buffer> f_buff_;
        std::unique_ptr<post_proc_buffer> pp_buff_;

        // Screen Meshes
        std::unique_ptr<mesh> screen_quad_;
        std::unique_ptr<mesh> skybox_mesh_;

        // Camera
        std::priority_queue<camera_data> cameras_;

        // Lights
        std::vector<light_data> lights_;

        // Meshes
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>> deferred_meshes_;
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>> forward_opaque_meshes_;
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<matrix4x4>>> forward_transparent_meshes_;

        graphics_renderer() {}
        virtual ~graphics_renderer() {}

        void render();
        void geometry_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix);
        void light_pass(const matrix4x4& _view_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right);
        void forward_pass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right, const skybox& _skybox);
        void post_proc_pass(float _near, float _far);

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