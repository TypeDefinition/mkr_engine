#pragma once

#include <maths/vector3.h>

namespace mkr {
    class bounding_box {
    private:
        const vector3 min_, max_;
        const vector3 centre_;

    public:
        bounding_box(const vector3& _min, const vector3& _max)
            : min_(_min), max_(_max), centre_((_min + _max) * 0.5f) {}

        virtual ~bounding_box() {}

        [[nodiscard]] const vector3& min() const { return min_; }

        [[nodiscard]] const vector3& max() const { return max_; }

        [[nodiscard]] const vector3& centre() const { return centre_; }
    };
}