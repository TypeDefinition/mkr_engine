#pragma once

#include <string>

namespace mkr {
    class scene;

    class system {
    protected:
        scene* const scene_;
        const std::string name_;

    public:
        system(scene* _scene, const std::string& _name = "System")
            : scene_(_scene), name_(_name) {}

        virtual ~system() = default;

        const std::string& name() const { return name_; }

        virtual void on_update(float _delta_time) {}
    };
}