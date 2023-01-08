#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace mkr {
    class file_util {
    public:
        file_util() = delete;

        static std::string file_to_str(const std::string& _filename) {
            std::ifstream is(_filename);
            std::stringstream ss;
            ss << is.rdbuf();
            return ss.str();
        }
    };
}