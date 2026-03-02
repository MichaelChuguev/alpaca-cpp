#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

struct Asset {

    std::string id;
    std::string asset_class;
    std::string exchange;
    std::string symbol;
    std::string name;
    std::string status;
    bool tradable;
    bool marginable;
    Decimal margin_requirement_long;
    Decimal margin_requirement_short;
    bool shortable;
    bool easy_to_borrow;
    bool fractionable;
    std::vector<std::string> attributes;

    Asset(const json& j)
    : 
        id(parse_string(j, "id")),
        asset_class(parse_string(j, "class")),
        exchange(parse_string(j, "exchange")),
        symbol(parse_string(j, "symbol")),
        name(parse_string(j, "name")),
        status(parse_string(j, "status")),
        tradable(parse_bool(j, "tradable")),
        marginable(parse_bool(j, "marginable")),
        margin_requirement_long(parse_decimal(j, "margin_requirement_long")),
        margin_requirement_short(parse_decimal(j, "margin_requirement_short")),
        shortable(parse_bool(j, "shortable")),
        easy_to_borrow(parse_bool(j, "easy_to_borrow")),
        fractionable(parse_bool(j, "fractionable"))
    {
        if (j.contains("attributes") && j["attributes"].is_array()) {
            attributes = j["attributes"].get<std::vector<std::string>>();
        } else {
            attributes = {};  // Default to empty vector if null or missing
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Asset Information:\n"
            << "ID: " << id << "\n"
            << "Asset Class: " << asset_class << "\n"
            << "Exchange: " << exchange << "\n"
            << "Symbol: " << symbol << "\n"
            << "Name: " << name << "\n"
            << "Status: " << status << "\n"
            << "Tradable: " << tradable << "\n"
            << "Marginable: " << marginable << "\n"
            << "Margin Requirement Long: " << margin_requirement_long << "\n"
            << "Margin Requirement Short: " << margin_requirement_short << "\n"
            << "Shortable: " << shortable << "\n"
            << "Easy To Borrow: " << easy_to_borrow << "\n"
            << "Fractionable: " << fractionable << "\n"
            << "Attributes: {\n";
        for (const auto& attribute : attributes) oss << "\t" << attribute << "\n";
        oss << "}";
            
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["class"] = asset_class;
        j["exchange"] = exchange;
        j["symbol"] = symbol;
        j["name"] = name;
        j["status"] = status;
        j["tradable"] = tradable;
        j["marginable"] = marginable;
        j["margin_requirement_long"] = margin_requirement_long.to_double();
        j["margin_requirement_short"] = margin_requirement_short.to_double();
        j["shortable"] = shortable;
        j["easy_to_borrow"] = easy_to_borrow;
        j["fractionable"] = fractionable;
        if (!attributes.empty()) {
            j["attributes"] = attributes;
        } else {
            j["attributes"] = nullptr; 
        }

        return j;
    }

};

} // namespace alpaca