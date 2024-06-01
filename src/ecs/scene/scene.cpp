#include <cassert>
#include <common/uuid.h>
#include "ecs/scene/scene.h"

namespace mkr {
    scene::scene(const std::string& _name)
        : name_(_name) {}

    scene::~scene() {
        // Delete pools.
        for (auto& iter : pools_) {
            delete iter.second;
        }
    }

    void scene::update(float _delta_time) {
        for (auto& system : systems_) {
            system->on_update(_delta_time);
        }
    }

    ecs_id_t scene::create_entity() {
        auto ent = ecs_id::instance().create_id();

        add_restricted<uuid>(ent);
        add_restricted<transform>(ent);
        add_restricted<local_to_world>(ent);
        add_restricted<root>(ent); // Root entities have no parents.

        parents_[ent] = ecs_id::invalid_id;
        children_[ent] = {};

        return ent;
    }

    void scene::destroy_entity(ecs_id_t _entity) {
        assert(ecs_id::instance().is_valid(_entity));

        // Destroy children.
        auto children = get_children(_entity);
        for (auto c : children) {
            destroy_entity(c);
        }

        // Detach from parent.
        remove_parent(_entity);

        // Delete all components.
        for (auto& p : pools_) {
            if (p.second->has_component(_entity)) {
                p.second->remove_component(_entity);
            }
        }
    }

    bool scene::has_parent(ecs_id_t _child) const {
        assert(ecs_id::instance().is_valid(_child));

        auto iter = parents_.find(_child);
        return iter != parents_.end() && iter->second != ecs_id::invalid_id;
    }

    void scene::set_parent(ecs_id_t _child, ecs_id_t _parent) {
        assert(ecs_id::instance().is_valid(_child));
        assert(ecs_id::instance().is_valid(_parent));

        remove_parent(_child);
        parents_[_child] = _parent;
        children_[_parent].push_back(_child);

        // The entity is now NOT a root entity since it has a parent.
        remove_restricted<root>(_child);
    }

    void scene::remove_parent(ecs_id_t _child) {
        if (!has_parent(_child)) { return; }

        auto parent = parents_[_child];
        parents_[_child] = ecs_id::invalid_id;

        // O(n) time removal, but since n is usually less than 10, this should not be an issue.
        auto& siblings = children_[parent];
        siblings.erase(std::ranges::find(siblings, _child));

        // The entity is now a root entity since it has no parent.
        add_restricted<root>(_child);
    }

    ecs_id_t scene::get_parent(ecs_id_t _child) const {
        assert(ecs_id::instance().is_valid(_child));

        auto iter = parents_.find(_child);
        return iter == parents_.end() ? ecs_id::invalid_id : iter->second;
    }

    std::vector<ecs_id_t> scene::get_children(ecs_id_t _parent) const {
        assert(ecs_id::instance().is_valid(_parent));

        auto iter = children_.find(_parent);
        return iter == children_.end() ? std::vector<ecs_id_t>{} : iter->second;
    }
} // mkr