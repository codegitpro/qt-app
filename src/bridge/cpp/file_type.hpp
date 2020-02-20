// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <functional>

namespace ai {

enum class FileType : int {
    IMAGE,
    TEXT,
    VIDEO,
    OTHER,
};

}  // namespace ai

namespace std {

template <>
struct hash<::ai::FileType> {
    size_t operator()(::ai::FileType type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std
