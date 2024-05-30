#pragma once

#include <utility>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mkr {
class log {
 private:
    static inline std::shared_ptr<spdlog::async_logger> core_logger_ = nullptr;
    static inline std::shared_ptr<spdlog::async_logger> client_logger_ = nullptr;

 public:
    log() = delete;

    static void init(const std::string& _log_file = "./log/log.txt") {
        spdlog::init_thread_pool(8192, 1);

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::level_enum::trace);

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_log_file, 5242880, 3, true);
        file_sink->set_level(spdlog::level::level_enum::trace);

        spdlog::sinks_init_list sink_list = {console_sink, file_sink};

        core_logger_ = std::make_shared<spdlog::async_logger>("Core Logger", sink_list.begin(), sink_list.end(), spdlog::thread_pool());
        core_logger_->set_level(spdlog::level::level_enum::trace); // Set the logger's log level to the lowest so that it does not override the sinks.
        core_logger_->flush_on(spdlog::level::level_enum::trace); // Set the logger's flush level to the lowest so that everything gets written to the file.

        client_logger_ = std::make_shared<spdlog::async_logger>("Client Logger", sink_list.begin(), sink_list.end(), spdlog::thread_pool());
        client_logger_->set_level(spdlog::level::level_enum::trace);
        client_logger_->flush_on(spdlog::level::level_enum::trace);
    }

    static void exit() {
        core_logger_->flush();
        core_logger_ = nullptr;
        client_logger_->flush();
        client_logger_ = nullptr;
    }

    inline static spdlog::async_logger* core_logger() { return core_logger_.get(); }

    inline static spdlog::async_logger* client_logger() { return client_logger_.get(); }
};
}

#ifndef NDEBUG
#define MKR_CORE_TRACE(...)    ::mkr::log::core_logger()->trace(__VA_ARGS__)
    #define MKR_CORE_DEBUG(...)    ::mkr::log::core_logger()->debug(__VA_ARGS__)
    #define MKR_CORE_INFO(...)     ::mkr::log::core_logger()->info(__VA_ARGS__)
    #define MKR_CORE_WARN(...)     ::mkr::log::core_logger()->warn(__VA_ARGS__)
    #define MKR_CORE_ERROR(...)    ::mkr::log::core_logger()->error(__VA_ARGS__)
    #define MKR_CORE_CRITICAL(...) ::mkr::log::core_logger()->critical(__VA_ARGS__)

    #define MKR_TRACE(...)    ::mkr::log::client_logger()->trace(__VA_ARGS__)
    #define MKR_DEBUG(...)    ::mkr::log::client_logger()->debug(__VA_ARGS__)
    #define MKR_INFO(...)     ::mkr::log::client_logger()->info(__VA_ARGS__)
    #define MKR_WARN(...)     ::mkr::log::client_logger()->warn(__VA_ARGS__)
    #define MKR_ERROR(...)    ::mkr::log::client_logger()->error(__VA_ARGS__)
    #define MKR_CRITICAL(...) ::mkr::log::client_logger()->critical(__VA_ARGS__)
#else
#define MKR_CORE_TRACE(...)
#define MKR_CORE_DEBUG(...)
#define MKR_CORE_INFO(...)
#define MKR_CORE_WARN(...)
#define MKR_CORE_ERROR(...)
#define MKR_CORE_CRITICAL(...)

#define MKR_TRACE(...)
#define MKR_DEBUG(...)
#define MKR_INFO(...)
#define MKR_WARN(...)
#define MKR_ERROR(...)
#define MKR_CRITICAL(...)
#endif
