// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form2.djinni

#pragma once

#include <functional>

namespace ai {

enum class DataType : int {
    TEXT,
    MULTILINE_TEXT,
    BOOLEAN,
    EMAIL,
    PASSWORD,
    REFERENCE,
    CHOICE,
    SKYPE,
    PHONE,
    CURRENCY,
    URL,
    INTEGER,
    RATING,
};

}  // namespace ai

namespace std {

template <>
struct hash<::ai::DataType> {
    size_t operator()(::ai::DataType type) const {
        return std::hash<int>()(static_cast<int>(type));
    }
};

}  // namespace std
