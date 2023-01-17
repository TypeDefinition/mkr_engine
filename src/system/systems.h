#pragma one

#include <functional>
#include <flecs.h>
#include "component/tag.h"
#include "component/transform.h"

namespace mkr {
    void calculate_transforms(flecs::entity _entity, const root& _root);
}