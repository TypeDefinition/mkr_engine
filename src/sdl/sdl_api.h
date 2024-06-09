#pragma once

namespace mkr {
    class sdl_api {
    private:
        static void exit_video();
        static void init_video();

        static void init_image();
        static void exit_image();

        static void init_input();
        static void exit_input();

        static void init_events();
        static void exit_events();

    public:
        sdl_api() = delete;

        static void init();
        static void exit();
    };
} // mkr