#pragma once

#include <string>

namespace mkr {
    struct tag {
        const std::string name_;
        tag(const std::string& _name) : name_(_name) {}
        virtual ~tag() {}
    };

    struct player_head : public tag {
        player_head() : tag("Player Head") {}
    };
    struct player_body : public tag {
        player_body() : tag("Player Body") {}
    };
} // mkr