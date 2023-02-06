#pragma once

#include "graphics/material.h"
#include "graphics/mesh.h"

namespace mkr {
    struct mesh_renderer {
        std::shared_ptr<material> material_;
        std::shared_ptr<mesh> mesh_;
    };
}