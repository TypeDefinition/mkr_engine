#pragma once

#include <functional>
#include <queue>
#include <flecs.h>
#include <common/singleton.h>
#include <maths/matrix_util.h>
#include "graphics/app_window.h"
#include "graphics/framebuffer.h"
#include "graphics/material.h"
#include "component/render_component.h"
#include "component/transform_component.h"
#include "component/camera_component.h"
#include "component/light_component.h"

namespace mkr {
    // There are 8 bits in the stencil buffer. We will use the higher 4-bits for our renderer.
    // The lower 4-bits will be reserved for the future.
    enum stencil_value : uint8_t {
        stencil_opaque_object = 0x10,
    };

    struct camera_data {
        transform_component transform_;
        camera_component camera_;

        inline bool operator<(const camera_data& _rhs) const {
            return camera_.depth_ < _rhs.camera_.depth_;
        }
    };

    struct light_data {
        transform_component transform_;
        light_component light_;
    };

    class renderer : public singleton<renderer> {
        friend class singleton<renderer>;

    private:
        std::unique_ptr<app_window> app_window_;
        std::shared_ptr<dbuffer> dbuffer_;
        std::shared_ptr<gbuffer> gbuffer_;
        std::shared_ptr<lbuffer> lbuffer_;
        std::shared_ptr<fbuffer> fbuffer_;
        std::shared_ptr<pbuffer> pbuffer_;
        std::shared_ptr<sbuffer> sbuffer_;
        std::shared_ptr<mesh> screen_quad_;
        std::shared_ptr<mesh> skybox_mesh_;

        // Camera
        std::priority_queue<camera_data> cameras_;

        // Lights
        std::vector<light_data> lights_;

        // Objects
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<mesh_instance_data>>> deferred_objs_;
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<mesh_instance_data>>> forward_opaque_objs_;
        std::unordered_map<material*, std::unordered_map<mesh*, std::vector<mesh_instance_data>>> forward_transparent_objs_;

        renderer() {}

        virtual ~renderer() {}

        void render();
        void gpass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix);
        void lpass(const matrix4x4& _view_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right);
        void fpass(const matrix4x4& _view_matrix, const matrix4x4& _projection_matrix, const vector3& _view_forward, const vector3& _view_up, const vector3& _view_right, std::shared_ptr<skybox> _skybox);
        void ppass(float _near, float _far);

    public:
        void init();

        void start();

        void update();

        void exit();

        void update_cameras(const transform_component& _transform, const camera_component& _camera);

        void update_lights(const transform_component& _transform, const light_component& _light);

        void update_objects(const transform_component& _transform, const render_component& _mesh_renderer);
    };
}