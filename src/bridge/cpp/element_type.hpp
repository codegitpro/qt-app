// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form.djinni

#pragma once

#include <functional>

namespace ai {

enum class ElementType : int {
    BUTTON,
    TEXT_INPUT,
    BOOL_INPUT,
    EMAIL_INPUT,
    LABEL,
    PASSWORD_INPUT,
    TEXT,
};

}  // namespace ai

namespace std {

template <>
struct hash<::ai::ElementType> {
    size_t operator()(::ai::ElementType type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std