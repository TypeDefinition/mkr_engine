#pragma once

#include <maths/maths_util.h>
#include <maths/matrix.h>
#include <maths/vector3.h>
#include "graphics/shadow/bounding_box.h"

namespace mkr {
    class shadow_bounds {
    public:
        shadow_bounds() = delete;

        /**
         * Get the shadow bounds for a perspective camera.
         * @param _light_view_matrix the "view" matrix of the light
         * @param _cam_pos the camera position
         * @param _cam_forward the NORMALISED direction the camera is facing
         * @param _cam_up the NORMALISED up direction of the camera
         * @param _cam_left the NORMALISED left direction of the camera
         * @param _near the camera's near clipping distance
         * @param _far the camera's far clipping distance
         * @param _aspect_ratio the camera's near aspect ratio
         * @param _fov the camera's field of view
         * @return the shadow's bounding box
         */
        static bounding_box get_perspective_bounds(const matrix4x4& _light_view_matrix,
                                                   const vector3& _cam_pos,
                                                   const vector3& _cam_left, const vector3& _cam_up, const vector3& _cam_forward,
                                                   float _near, float _far,
                                                   float _aspect_ratio, float _fov) {
            const float far_height = _far * std::tan(_fov * 0.5f);
            const float far_width = far_height * _aspect_ratio;
            const float near_height = _near * std::tan(_fov * 0.5f);
            const float near_width = near_height * _aspect_ratio;

            // Transform points into light space.
            const auto cam_pos = _light_view_matrix * _cam_pos;
            const auto cam_left = _light_view_matrix * _cam_left;
            const auto cam_up = _light_view_matrix * _cam_up;
            const auto cam_forward = _light_view_matrix * _cam_forward;

            // Find the corners of the view frustum.
            const vector3 near_centre = cam_pos + cam_forward * _near;
            const vector3 far_centre = cam_pos + cam_forward * _far;

            const vector3 near_top_left = near_centre + near_width * cam_left + near_height * cam_up;
            const vector3 near_top_right = near_centre + near_width * -cam_left + near_height * cam_up;
            const vector3 near_bottom_left = near_centre + near_width * cam_left + near_height * -cam_up;
            const vector3 near_bottom_right = near_centre + near_width * -cam_left + near_height * -cam_up;

            const vector3 far_top_left = far_centre + far_width * cam_left + far_height * cam_up;
            const vector3 far_top_right = far_centre + far_width * -cam_left + far_height * cam_up;
            const vector3 far_bottom_left = far_centre + far_width * cam_left + far_height * -cam_up;
            const vector3 far_bottom_right = far_centre + far_width * -cam_left + far_height * -cam_up;

            vector3 points[8] = {near_top_left, near_top_right, near_bottom_left, near_bottom_right,
                                 far_top_left, far_top_right, far_bottom_left, far_bottom_right};

            // Find the min and max bounds.
            vector3 min_bounds = points[0];
            vector3 max_bounds = points[0];
            for (auto i = 1; i < 8; ++i) {
                const auto& point = points[i];
                min_bounds.x_ = maths_util::min<float>(min_bounds.x_, point.x_);
                min_bounds.y_ = maths_util::min<float>(min_bounds.y_, point.y_);
                min_bounds.z_ = maths_util::min<float>(min_bounds.z_, point.z_);

                max_bounds.x_ = maths_util::max<float>(max_bounds.x_, point.x_);
                max_bounds.y_ = maths_util::max<float>(max_bounds.y_, point.y_);
                max_bounds.z_ = maths_util::max<float>(max_bounds.z_, point.z_);
            }

            return bounding_box{min_bounds, max_bounds};
        }

        /**
         * Get the shadow bounds for a perspective camera.
         * @param _light_view_matrix the "view" matrix of the light
         * @param _cam_pos the camera position
         * @param _cam_forward the NORMALISED direction the camera is facing
         * @param _cam_up the NORMALISED up direction of the camera
         * @param _cam_left the NORMALISED left direction of the camera
         * @param _near the camera's near clipping distance
         * @param _far the camera's far clipping distance
         * @param _aspect_ratio the camera's near aspect ratio
         * @param _ortho_size size of the camera's vertical view volume
         * @return the shadow's bounding box
         */
        static bounding_box get_orthographic_bounds(const matrix4x4& _light_view_matrix,
                                                    const vector3& _cam_pos,
                                                    const vector3& _cam_forward, const vector3& _cam_up, const vector3& _cam_left,
                                                    float _near, float _far,
                                                    float _aspect_ratio, float _ortho_size) {
            const float height = _ortho_size * 0.5f;
            const float width = height * _aspect_ratio;

            // Transform points into light space.
            const auto cam_pos = _light_view_matrix * _cam_pos;
            const auto cam_left = _light_view_matrix * _cam_left;
            const auto cam_up = _light_view_matrix * _cam_up;
            const auto cam_forward = _light_view_matrix * _cam_forward;

            // Find the corners of the view cuboid.
            const vector3 near_centre = cam_pos + cam_forward * _near;
            const vector3 far_centre = cam_pos + cam_forward * _far;

            const vector3 near_top_left = near_centre + width * cam_left + height * cam_up;
            const vector3 near_top_right = near_centre + width * -cam_left + height * cam_up;
            const vector3 near_bottom_left = near_centre + width * cam_left + height * -cam_up;
            const vector3 near_bottom_right = near_centre + width * -cam_left + height * -cam_up;

            const vector3 far_top_left = far_centre + width * cam_left + height * cam_up;
            const vector3 far_top_right = far_centre + width * -cam_left + height * cam_up;
            const vector3 far_bottom_left = far_centre + width * cam_left + height * -cam_up;
            const vector3 far_bottom_right = far_centre + width * -cam_left + height * -cam_up;

            vector3 points[8] = {near_top_left, near_top_right, near_bottom_left, near_bottom_right,
                                 far_top_left, far_top_right, far_bottom_left, far_bottom_right};

            // Find the min and max bounds.
            vector3 min_bounds = points[0];
            vector3 max_bounds = points[0];
            for (auto i = 1; i < 8; ++i) {
                const auto& point = points[i];
                min_bounds.x_ = maths_util::min<float>(min_bounds.x_, point.x_);
                min_bounds.y_ = maths_util::min<float>(min_bounds.y_, point.y_);
                min_bounds.z_ = maths_util::min<float>(min_bounds.z_, point.z_);

                max_bounds.x_ = maths_util::max<float>(max_bounds.x_, point.x_);
                max_bounds.y_ = maths_util::max<float>(max_bounds.y_, point.y_);
                max_bounds.z_ = maths_util::max<float>(max_bounds.z_, point.z_);
            }

            return bounding_box{min_bounds, max_bounds};
        }
    };
}