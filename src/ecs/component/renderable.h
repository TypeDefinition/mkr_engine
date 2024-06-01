#pragma once

#include <string>
#include "graphics/material/material.h"
#include "graphics/mesh/mesh.h"

namespace mkr {
    struct renderable {
        material* material_;
        mesh* mesh_;
    };
}