// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

#pragma once

#include <string>
#include <utility>

namespace ai {

struct Product final {
    std::string product_id;
    std::string title;
    std::string product_description;
    float price;
    std::string formatted_price;

    Product(std::string product_id_,
            std::string title_,
            std::string product_description_,
            float price_,
            std::string formatted_price_)
    : product_id(std::move(product_id_))
    , title(std::move(title_))
    , product_description(std::move(product_description_))
    , price(std::move(price_))
    , formatted_price(std::move(formatted_price_))
    {}
};

}  // namespace ai