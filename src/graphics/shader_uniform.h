#pragma once

#include <array>
#include <string>

namespace mkr {
    // N must be between 1 and 4 inclusive.
    template<typename T, size_t N = 1>
    class shader_uniform {
    public:
        const std::string uniform_name_;
        const std::array<T, N> values_;

        shader_uniform() = delete;

        explicit shader_uniform(const std::string& _uniform_name, const T& _value) requires (N == 1)
                : uniform_name_(_uniform_name), values_({_value}) {}

        explicit shader_uniform(const std::string& _uniform_name, const std::array<T, N>& _values) requires (1 <= N && N <= 4)
                : uniform_name_(_uniform_name), values_(_values) {}

        virtual ~shader_uniform() = default;
    };
}