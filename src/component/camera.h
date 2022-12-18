#pragma once

#include <memory>
#include "graphics/shader_program.h"
#include "graphics/texture.h"

namespace mkr {
    enum projection_mode {
        perspective = 0,
        orthographic,
    };

    struct viewport {
        // (0, 0) represents the bottom left of the screen and (1, 1) represents the top right of the screen.
        float bottom_x_, bottom_y_, top_x_, top_y_;

        viewport(float _bottom_x = 0.0f, float _bottom_y = 0.0f, float _top_x = 1.0f, float _top_y = 1.0f)
                : bottom_x_(_bottom_x), bottom_y_(_bottom_y), top_x_(_top_x), top_y_(_top_y) {
        }

        ~viewport() {}
    };

    struct camera {
        /// Projection mode
        projection_mode mode_ = projection_mode::perspective;
        /// Near plane
        float near_plane_ = 0.3f;
        /// Far plane
        float far_plane_ = 500.0f;
        /// Aspect ratio
        float aspect_ratio_ = 16.0f / 9.0f;
        /// Perspective field of view
        float fov_ = 45.0F;
        /// Orthographic Size
        float ortho_size_ = 5.0F;
        /// Camera depth
        uint8_t depth_ = 0;
        /// Viewport
        viewport viewport_;

        /// Deferred lighting shader
        std::shared_ptr<shader_program> lighting_shader_;
        /// Post processing shaders
        std::vector<std::shared_ptr<shader_program>> post_process_shaders_;

        /// Skybox shader
        std::shared_ptr<shader_program> skybox_shader_;
        /// Skybox colour
        colour skybox_colour_ = colour::white;
        /// Skybox texture
        std::shared_ptr<texture> skybox_texture_;
    };
}