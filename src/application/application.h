#pragma once

#include <memory>
#include <atomic>
#include <common/singleton.h>
#include "ecs/scene/scene.h"

namespace mkr {
    class application : public singleton<application> {
        friend class singleton<application>;

    private:
        /// The duration taken for the current frame.
        float delta_time_ = 0.0f;
        /// The duration that has passed since the application started.
        float time_elapsed_ = 0.0f;
        /// A flag to exit the game loop. Set to false to quit the application.
        std::atomic_bool run_ = true;

        std::unique_ptr<scene> scene_;

        void init();
        void update();
        void exit();

    public:
        application() {}
        virtual ~application() {}

        inline float delta_time() const { return delta_time_; }
        inline float time_elapsed() const { return time_elapsed_; }
        inline void quit() { run_ = false; }
        virtual void run();
    };
} // mkr