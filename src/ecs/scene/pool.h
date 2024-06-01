#pragma once

#include <cassert>
#include <vector>
#include <memory>
#include <unordered_map>
#include <roaring/roaring.hh>
#include "ecs/ecs_id.h"

namespace mkr {
    class pool_base {
    protected:
        roaring::Roaring bitmap_;
        std::unordered_map<ecs_id_t, std::size_t> entity_to_index_;
        std::vector<ecs_id_t> index_to_entity_;

    public:
        pool_base() {}
        virtual ~pool_base() {}

        inline const roaring::Roaring& get_bitmap() const { return bitmap_; }

        // For entity destruction.
        virtual bool has_component(ecs_id_t _entity) const = 0;
        virtual void remove_component(ecs_id_t _entity) = 0;
    };

    template <typename T>
    class pool : public pool_base {
    private:
        std::vector<std::unique_ptr<T>> components_;

    public:
        pool() {}

        virtual ~pool() {}

        inline std::size_t size() const { return components_.size(); }

        inline const std::vector<ecs_id_t>& get_entities() const { return index_to_entity_; };

        bool has_component(ecs_id_t _entity) const {
            return entity_to_index_.find(_entity) != entity_to_index_.end();
        }

        T* get_component(ecs_id_t _entity) {
            assert(has_component(_entity));

            return components_[entity_to_index_.find(_entity)->second].get();
        }

        const T* get_component(ecs_id_t _entity) const {
            assert(has_component(_entity));

            return components_[entity_to_index_.find(_entity)->second].get();
        }

        template <typename... Args>
        T* add_component(ecs_id_t _entity, Args&&... _args) {
            assert(!has_component(_entity));

            bitmap_.add(ecs_id::index_of(_entity));
            entity_to_index_[_entity] = components_.size();
            index_to_entity_.push_back(_entity);
            components_.push_back(std::make_unique<T>(std::forward<Args>(_args)...));
            return components_.back().get();
        }

        void remove_component(ecs_id_t _entity) {
            assert(has_component(_entity));

            bitmap_.remove(ecs_id::index_of(_entity));
            auto index = entity_to_index_[_entity]; // Index of component to be deleted.
            auto last_entity = index_to_entity_.back(); // Entity of the last component.
            auto last_index = entity_to_index_[last_entity]; // Index of the last component.

            // Swap the component to be deleted and the last component, so that we can just call pop_back() to delete the last component.
            std::swap(entity_to_index_[_entity], entity_to_index_[last_entity]);
            std::swap(index_to_entity_[index], index_to_entity_[last_index]);
            std::swap(components_[index], components_[last_index]);

            // Delete component.
            entity_to_index_.erase(_entity);
            index_to_entity_.pop_back();
            components_.pop_back();
        }
    };
} // mkr