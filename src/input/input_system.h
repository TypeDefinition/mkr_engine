#pragma once

#include <common/singleton.h>

namespace mkr {
    class input_system : public singleton<input_system> {
    private:
        input_system() = default;
        virtual ~input_system() = default;

    public:
    };
}