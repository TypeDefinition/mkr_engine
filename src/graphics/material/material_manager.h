#pragma once

#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include "graphics/material/material.h"

namespace mkr {
    class material_manager : public singleton<material_manager> {
        friend class singleton<material_manager>;

    private:
        std::unordered_map<std::string, std::unique_ptr<material>> materials_;

        material_manager() {}

        virtual ~material_manager() {}

    public:
        material* get_material(const std::string& _name) {
            auto iter = materials_.find(_name);
            return (iter == materials_.end()) ? nullptr : iter->second.get();
        }

        material* make_material(const std::string& _name) {
            materials_[_name] = std::make_unique<material>();;
            return materials_[_name].get();
        }
    };
}