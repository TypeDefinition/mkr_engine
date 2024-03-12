#pragma once

#include <string>
#include "graphics/material/material.h"
#include "graphics/mesh/mesh.h"


namespace mkr {
    struct render_mesh {
        material* material_;
        mesh* mesh_;
    };
}