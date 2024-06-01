#pragma once

#include <maths/vector3.h>
#include <maths/quaternion.h>
#include <maths/matrix_util.h>
#include "ecs/component/restricted.h"

namespace mkr {
    class transform : public restricted {
    private:
        vector3 position_;
        quaternion rotation_;
        vector3 scale_;

    public:
        transform(const vector3& _position = vector3::zero(),
                  const quaternion& _rotation = quaternion(),
                  const vector3& _scale = {1.0f, 1.0f, 1.0f})
                : position_(_position), rotation_(_rotation), scale_(_scale) {}

        virtual ~transform() = default;

        [[nodiscard]] inline const vector3& get_position() const { return position_; }

        transform& set_position(const vector3& _position) {
            position_ = _position;
            return *this;
        }

        transform& translate(const vector3& _translation) {
            position_ += _translation;
            return *this;
        }

        [[nodiscard]] inline const quaternion& get_rotation() const { return rotation_; }

        transform& set_rotation(const quaternion& _rotation) {
            rotation_ = _rotation;
            return *this;
        }

        transform& rotate(const quaternion& _rotation) {
            rotation_ = _rotation * rotation_;
            return *this;
        }

        [[nodiscard]] inline const vector3& get_scale() const { return scale_; }

        transform& set_scale(const vector3& _scale) {
            scale_ = _scale;
            return *this;
        }

        transform& scale(float _scalar) {
            scale_ *= _scalar;
            return *this;
        }

        [[nodiscard]] inline vector3 left() const { return quaternion::rotate(vector3::x_axis(), rotation_); }

        [[nodiscard]] inline vector3 up() const { return quaternion::rotate(vector3::y_axis(), rotation_); }

        [[nodiscard]] inline vector3 forward() const { return quaternion::rotate(vector3::z_axis(), rotation_); }

        [[nodiscard]] inline matrix4x4 translation_matrix() const { return matrix_util::translation_matrix(position_); }

        [[nodiscard]] inline matrix4x4 rotation_matrix() const { return rotation_.to_rotation_matrix(); }

        [[nodiscard]] inline matrix4x4 scale_matrix() const { return matrix_util::scale_matrix(scale_); }

        [[nodiscard]] inline matrix4x4 transform_matrix() const { return translation_matrix() * rotation_matrix() * scale_matrix(); }
    };
}