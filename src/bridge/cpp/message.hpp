// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace ai {

struct Message final {
    int32_t field;
    std::string content;

    Message(int32_t field_,
            std::string content_)
    : field(std::move(field_))
    , content(std::move(content_))
    {}
};

}  // namespace ai
