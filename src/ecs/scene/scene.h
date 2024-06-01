#pragma once

#include <vector>
#include <utility>
#include <memory>
#include <type_traits>
#include <string>
#include <unordered_map>
#include <common/type_id.h>
#include "ecs/ecs_id.h"
#include "ecs/scene/pool.h"
#include "ecs/system/system.h"
#include "ecs/component/restricted.h"
#include "ecs/component/tags.h"
#include "ecs/component/transform.h"
#include "ecs/component/local_to_world.h"

namespace mkr {
    class scene {
    private:
        std::string name_;
        std::unordered_map<ecs_id_t /* Child */, ecs_id_t /* Parent */> parents_;
        std::unordered_map<ecs_id_t /* Parent */, std::vector<ecs_id_t> /* Children */> children_;

        std::unordered_map<type_id_t, pool_base*> pools_;
        std::vector<std::unique_ptr<system>> systems_;

        template <typename T, typename... Args>
        T* add_restricted(ecs_id_t _entity, Args&& ... _args) {
            assert(!all_of<T>(_entity));
            return get_or_create_pool<T>()->add_component(_entity, std::forward<Args>(_args)...);
        }

        template <typename T>
        void remove_restricted(ecs_id_t _entity) {
            assert(all_of<T>(_entity));
            return get_pool<T>()->remove_component(_entity);
        }

        template <typename T>
        pool<T>* get_or_create_pool() {
            const auto tid = MKR_TYPE_ID(scene, T);
            if (!pools_.contains(tid)) {
                pools_[tid] = new pool<T>();
            }
            return (pool<T>*) pools_[tid];
        }

        template <typename T>
        pool<T>* get_pool() {
            auto iter = pools_.find(MKR_TYPE_ID(scene, T));
            return iter == pools_.end() ? nullptr : (pool<T>*) iter->second;
        }

        template <typename T>
        const pool<T>* get_pool() const {
            auto iter = pools_.find(MKR_TYPE_ID(scene, T));
            return iter == pools_.end() ? nullptr : (const pool<T>*) iter->second;
        }

        template <typename T>
        roaring::Roaring get_bitmap() const {
            auto iter = pools_.find(MKR_TYPE_ID(scene, T));
            return iter == pools_.end() ? roaring::Roaring{} : iter->second->get_bitmap();
        }

        template <typename T, typename U, typename... V>
        roaring::Roaring get_bitmap() const {
            return get_bitmap<T>() &= get_bitmap<U, V...>();
        }

    public:
        scene(const std::string& _name = "Scene");

        virtual ~scene();

        const std::string& name() const { return name_; }

        virtual void init() {}

        virtual void update(float _delta_time);

        virtual void exit() {}

        ecs_id_t create_entity();

        void destroy_entity(ecs_id_t _entity);

        bool has_parent(ecs_id_t _child) const;

        void set_parent(ecs_id_t _child, ecs_id_t _parent);

        void remove_parent(ecs_id_t _child);

        ecs_id_t get_parent(ecs_id_t _child) const;

        std::vector<ecs_id_t> get_children(ecs_id_t _parent) const;

        template <typename T, typename... Args>
        void add_system(Args&& ... _args) {
            systems_.push_back(std::make_unique<T>(this, std::forward<Args>(_args)...));
        }

        template <typename T>
        std::vector<ecs_id_t> entities_with() const {
            auto pool = get_pool<T>();
            return pool == nullptr ? std::vector<ecs_id_t>{} : pool->get_entities();
        }

        template <typename T, typename U, typename... V>
        std::vector<ecs_id_t> entities_with() const {
            auto bitmap = get_bitmap<T, U, V...>();
            std::vector<ecs_id_t> entities;
            entities.reserve(bitmap.cardinality());
            for (auto iter = bitmap.begin(); iter != bitmap.end(); ++iter) {
                entities.push_back(ecs_id::instance().get_from_index(*iter));
            }
            return entities;
        }

        template <typename T>
        bool all_of(ecs_id_t _entity) const {
            auto pool = get_pool<T>();
            return pool && pool->has_component(_entity);
        }

        template <typename T, typename U, typename... Args>
        bool all_of(ecs_id_t _entity) const {
            return all_of<T>(_entity) && all_of<U, Args...>(_entity);
        }

        template <typename T>
        bool any_of(ecs_id_t _entity) const {
            auto pool = get_pool<T>();
            return pool && pool->has_component(_entity);
        }

        template <typename T, typename U, typename... V>
        bool any_of(ecs_id_t _entity) const {
            return any_of<T>(_entity) || any_of<U, V...>(_entity);
        }

        template <typename T>
        const T* get_component(ecs_id_t _entity) const {
            assert(all_of<T>(_entity));
            return get_pool<T>()->get_component(_entity);
        }

        template <typename T>
        T* get_component(ecs_id_t _entity) {
            assert(all_of<T>(_entity));
            return get_pool<T>()->get_component(_entity);
        }

        template <typename T, typename... Args>
        T* add_component(ecs_id_t _entity, Args&& ... _args) requires (!std::is_base_of_v<restricted, T>) {
            return add_restricted<T>(_entity, std::forward<Args>(_args)...);
        }

        template <typename T>
        void remove_component(ecs_id_t _entity) requires (!std::is_base_of_v<restricted, T>) {
            return remove_restricted<T>(_entity);
        }
    };
} // mkr