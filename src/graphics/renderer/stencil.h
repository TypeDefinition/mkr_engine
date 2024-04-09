#pragma once

#include <cstdint>

namespace mkr {
    // There are 8 bits in the stencil buffer.
    enum stencil : uint8_t {
        placeholder0 = 1,
        placeholder1 = 2,
        placeholder2 = 4,
        placeholder3 = 8,
        placeholder4 = 16,
        placeholder5 = 32,
        placeholder6 = 64,
        placeholder7 = 128,
    };
}