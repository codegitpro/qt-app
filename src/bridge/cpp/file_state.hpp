// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <functional>

namespace ai {

enum class FileState : int {
    NORMAL,
    DOWNLOADING,
    UPLOADING,
    FAILED,
    LOCAL,
    DELETING,
    DELETED,
};

}  // namespace ai

namespace std {

template <>
struct hash<::ai::FileState> {
    size_t operator()(::ai::FileState type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std
