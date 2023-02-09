#pragma once

#include <functional>
#include <queue>
#include <flecs.h>
#include <common/singleton.h>
#include <maths/matrix_util.h>
#include "graphics/app_window.h"
#include "graphics/framebuffer.h"
#include "graphics/material.h"
#include "component/mesh_renderer.h"
#include "component/transform.h"
#include "component/camera.h"
#include "component/light.h"
#include "component/tag.h"

namespace mkr {
    // There are 8 bits in the stencil buffer. We will use the higher 4-bits for our renderer.
    // The lower 4-bits will be reserved for the future.
    enum stencil_value : uint8_t {
        stencil_opaque_object = 0x10,
    };

    struct camera_data {
        global_transform transform_;
        camera camera_;

        inline bool operator<(const camera_data& _rhs) const {
            return camera_.depth_ < _rhs.camera_.depth_;
        }
    };

    struct light_data {
        global_transform transform_;
        light light_;
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
        std::shared_ptr<mesh> screen_quad_;
        std::shared_ptr<mesh> skybox_mesh_;

        // Camera
        std::priority_queue<camera_data> cameras_;

        // Lights
        std::vector<light_data> lights_;

        // Objects
        std::unordered_map<std::shared_ptr<material>, std::unordered_map<std::shared_ptr<mesh>, std::vector<mesh_instance>>> deferred_objs_;
        std::unordered_map<std::shared_ptr<material>, std::unordered_map<std::shared_ptr<mesh>, std::vector<mesh_instance>>> forward_opaque_objs_;
        std::unordered_map<std::shared_ptr<material>, std::unordered_map<std::shared_ptr<mesh>, std::vector<mesh_instance>>> forward_transparent_objs_;

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

        void update_cameras(const global_transform& _global_transform, const camera& _camera);

        void update_lights(const global_transform& _global_transform, const light& _light);

        void update_objects(const global_transform& _global_transform, const mesh_renderer& _mesh_renderer);
    };
}