#pragma once

#include <memory>
#include <unordered_map>
#include <common/singleton.h>
#include "graphics/mesh/mesh.h"
#include "graphics/mesh/mesh_builder.h"
#include "util/file_util.h"

namespace mkr {
    class mesh_manager : public singleton<mesh_manager> {
        friend class singleton<mesh_manager>;

    private:
        std::unordered_map<std::string, std::unique_ptr<mesh>> meshes_;

        mesh_manager() {}

        virtual ~mesh_manager() {}

    public:
        mesh* get_mesh(const std::string& _name) {
            auto iter = meshes_.find(_name);
            return (iter == meshes_.end()) ? nullptr : iter->second.get();
        }

        // For now, we only support .obj files.
        mesh* make_mesh(const std::string& _name, const std::string& _file) {
            meshes_[_name] = mesh_builder::load_obj(_name, _file);
            return meshes_[_name].get();
        }
    };
}