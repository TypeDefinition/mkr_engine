#pragma once

#include <atomic>
#include <string>
#include <common/singleton.h>

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

        application() {}
        virtual ~application() {}

        void init();
        void start();
        void update();
        void stop();
        void exit();

    public:
        inline float delta_time() const { return delta_time_; }
        inline float time_elapsed() const { return time_elapsed_; }
        inline void terminate() { run_ = false; }
        virtual void run();
    };
} // mkr