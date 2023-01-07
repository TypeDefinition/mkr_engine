#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace mkr {
    class file_util {
    public:
        file_util() = delete;

        static std::string file_to_str(const std::string& _file) {
            std::ifstream is(_file);
            std::stringstream ss;
            ss << is.rdbuf();
            return ss.str();
        }
    };
}