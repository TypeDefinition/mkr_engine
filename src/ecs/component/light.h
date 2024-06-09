#pragma once

#include <maths/maths_util.h>
#include <maths/colour.h>

namespace mkr {
    enum light_mode {
        point = 0,
        spot,
        directional,
    };

    class light {
    private:
        light_mode mode_ = light_mode::point;
        colour colour_ = colour::white();
        float power_ = 1.0f;

        float attenuation_constant_ = 1.0f;
        float attenuation_linear_ = 0.0045f;
        float attenuation_quadratic_ = 0.0075f;

        float spotlight_inner_angle_ = maths_util::deg2rad * 60.0f;
        float spotlight_outer_angle_ = maths_util::deg2rad * 90.0f;

        float shadow_distance_ = 50.0f; // Only for spot and point light. Maximum distance that the light can cast a shadow.

    public:
        light() = default;

        virtual ~light() = default;

        inline light_mode get_mode() const { return mode_; }

        inline void set_mode(light_mode _type) { mode_ = _type; }

        inline float get_power() const { return power_; }

        inline void set_power(float _power) { power_ = maths_util::max<float>(0.0f, _power); }

        inline void set_colour(const colour& _colour) { colour_ = _colour; }

        inline const colour& get_colour() const { return colour_; }

        inline float get_attenuation_constant() const { return attenuation_constant_; }

        inline void set_attenuation_constant(float _attenuation) { attenuation_constant_ = maths_util::max<float>(_attenuation, 0.0f); }

        inline float get_attenuation_linear() const { return attenuation_linear_; }

        inline void set_attenuation_linear(float _attenuation) { attenuation_linear_ = maths_util::max<float>(_attenuation, 0.0f); }

        inline float get_attenuation_quadratic() const { return attenuation_quadratic_; }

        inline void set_attenuation_quadratic(float _attenuation) { attenuation_quadratic_ = maths_util::max<float>(_attenuation, 0.0f); }

        inline float get_spotlight_inner_consine() const { return std::cos(0.5f * spotlight_inner_angle_); }

        inline float get_spotlight_inner_angle() const { return spotlight_inner_angle_; }

        inline void set_spotlight_inner_angle(float _radians) { spotlight_inner_angle_ = maths_util::clamp<float>(_radians, maths_util::deg2rad, maths_util::deg2rad * 179.0f); }

        inline float get_spotlight_outer_consine() const { return std::cos(0.5f * spotlight_outer_angle_); }

        inline float get_spotlight_outer_angle() const { return spotlight_outer_angle_; }

        inline void set_spotlight_outer_angle(float _radians) { spotlight_outer_angle_ = maths_util::clamp<float>(_radians, maths_util::deg2rad, maths_util::deg2rad * 179.0f); }

        inline float get_shadow_distance() const { return shadow_distance_; }

        inline void set_shadow_distance(float _distance) { shadow_distance_ = _distance; }
    };
}