// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from form2.djinni

#pragma once

#include "block.hpp"
#include "data_type.hpp"
#include "field_type.hpp"
#include "style.hpp"
#include <cstdint>
#include <string>
#include <utility>

namespace ai {

struct Field final {
    Block rect;
    int32_t field_id;
    DataType data_t;
    FieldType field_t;
    std::string title;
    bool required;
    Style style;

    Field(Block rect_,
          int32_t field_id_,
          DataType data_t_,
          FieldType field_t_,
          std::string title_,
          bool required_,
          Style style_)
    : rect(std::move(rect_))
    , field_id(std::move(field_id_))
    , data_t(std::move(data_t_))
    , field_t(std::move(field_t_))
    , title(std::move(title_))
    , required(std::move(required_))
    , style(std::move(style_))
    {}
};

}  // namespace ai
