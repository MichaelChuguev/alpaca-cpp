#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "nlohmann/json.hpp"

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
    double margin_requirement_long;
    double margin_requirement_short;
    bool shortable;
    bool easy_to_borrow;
    bool fractionable;
    std::vector<std::string> attributes;

    Asset(const json& j)
    : 
        id(j["id"].get<std::string>()),
        asset_class(j["class"].get<std::string>()),
        exchange(j["exchange"].get<std::string>()),
        symbol(j["symbol"].get<std::string>()),
        name(j["name"].get<std::string>()),
        status(j["status"].get<std::string>()),
        tradable(j["tradable"].get<bool>()),
        marginable(j["marginable"].get<bool>()),
        margin_requirement_long(std::stod(j["margin_requirement_long"].get<std::string>())),
        margin_requirement_short(std::stod(j["margin_requirement_short"].get<std::string>())),
        shortable(j["shortable"].get<bool>()),
        easy_to_borrow(j["easy_to_borrow"].get<bool>()),
        fractionable(j["fractionable"].get<bool>())
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
        j["margin_requirement_long"] = margin_requirement_long;
        j["margin_requirement_short"] = margin_requirement_short;
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