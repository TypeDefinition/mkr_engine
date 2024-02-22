#pragma once

#include <utility>

namespace mkr {
    template<typename... Args>
    class system {
    protected:
        virtual void execute(Args&... _args) = 0;

    public:
        system() = default;
        virtual ~system() = default;

        void operator()(Args&& ... _args) { execute(std::forward(_args...)); }
    };
} // mkr