#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

enum OptionContractType {
    CALL,
    PUT
};

enum OptionContractStyle {
    AMERICAN,
    EUROPEAN
};

enum OptionContractStatus {
    ACTIVE,
    INACTIVE
};

struct OptionContract {

    struct Deliverable {
        std::string type;
        std::string symbol;
        std::string asset_id;
        double amount;
        double allocation_percentage;
        std::string settlement_type;
        std::string settlement_method;
        bool delayed_settlement;

        Deliverable(const json& j) 
        :
            type(j["type"]),
            symbol(j["symbol"]),
            asset_id(j["asset_id"]),
            amount(std::stod(j["amount"].get<std::string>())),
            allocation_percentage(std::stod(j["allocation_percentage"].get<std::string>())),
            settlement_type(j["settlement_type"]),
            settlement_method(j["settlement_method"]),
            delayed_settlement(j["delayed_settlement"])
        {
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << "Deliverable: {\n"
                << "\tType: " << type << "\n"
                << "\tSymbol: " << symbol << "\n"
                << "\tAsset ID: " << asset_id << "\n"
                << "\tAmount: " << amount << "\n"
                << "\tAllocation Percentage: " << allocation_percentage << "\n"
                << "\tSettlement Type: " << settlement_type << "\n"
                << "\tSettlement Method: " << settlement_method << "\n"
                << "\tDelayed Settlement: " << delayed_settlement << "\n}";

            return oss.str();
        }

        json to_json() const {
            json j;
            j["type"] = type;
            j["symbol"] = symbol;
            j["asset_id"] = asset_id;
            j["amount"] = amount;
            j["allocation_percentage"] = allocation_percentage;
            j["settlement_type"] = settlement_type;
            j["settlement_method"] = settlement_method;
            j["delayed_settlement"] = delayed_settlement;

            return j;
        }
    };

    std::string id;
    std::string symbol;
    std::string name;
    std::string status;
    bool tradable;
    DateTime expiration_date;
    std::string root_symbol;
    std::string underlying_symbol;
    std::string underlying_asset_id;
    std::string type;
    std::string style;
    double strike_price;
    double multiplier;
    double size;
    double open_interest;
    DateTime open_interest_date;
    double close_price;
    DateTime close_price_date;
    std::vector<Deliverable> deliverables;
    std::string next_page_token;

    OptionContract(const json& j)
    :
        id(j.value("id", "")),
        symbol(j.value("symbol", "")),
        name(j.value("name", "")),
        status(j.value("status", "")),
        tradable(j.value("tradable", false)),
        expiration_date(DateTime::parse(j.value("expiration_date", ""))),
        root_symbol(j.value("root_symbol", "")),
        underlying_symbol(j.value("underlying_symbol", "")),
        underlying_asset_id(j.value("underlying_asset_id", "")),
        type(j.value("type", "")),
        style(j.value("style", "")),
        strike_price(j.contains("strike_price") ? stod(j["strike_price"].get<std::string>()) : 0.0),
        multiplier(j.contains("multiplier") ? stod(j["multiplier"].get<std::string>()) : 0.0),
        size(j.contains("size") ? stod(j["size"].get<std::string>()) : 0.0),
        open_interest(j.contains("open_interest") ? stod(j["open_interest"].get<std::string>()) : 0.0),
        open_interest_date(DateTime::parse(j.value("open_interest_date", ""))),
        close_price(j.contains("close_price") ? stod(j["close_price"].get<std::string>()) : 0.0),
        close_price_date(DateTime::parse(j.value("close_price_date", ""))),
        next_page_token(j.value("next_page_token", ""))
    {
        if (j.contains("deliverables") && j["deliverables"].is_array()) {
            for (const auto& deliv : j["deliverables"]) deliverables.emplace_back(deliv);
        }
    }

    std::string to_string() const{
        std::ostringstream oss;
        oss << "Option Contract:\n"
            << "ID: " << id << "\n"
            << "Symbol: " << symbol << "\n"
            << "Name: " << name << "\n"
            << "Status: " << status << "\n"
            << "Tradable: " << tradable << "\n"
            << "Expiration Date: " << expiration_date.to_string() << "\n"
            << "Root Symbol: " << root_symbol << "\n"
            << "Underlying Symbol: " << underlying_symbol << "\n"
            << "Underlying Asset ID: " << underlying_asset_id << "\n"
            << "Type: " << type << "\n"
            << "Style: " << style << "\n"
            << "Strike Price: " << strike_price << "\n"
            << "Multiplier: " << multiplier << "\n"
            << "Size: " << size << "\n"
            << "Open Interest: " << open_interest << "\n"
            << "Open Interest Date: " << open_interest_date.to_string() << "\n"
            << "Close Price: " << close_price << "\n"
            << "Close Price Date: " << close_price_date.to_string() << "\n"
            << "Deliverables: {\n";
        for (const auto& deliverable : deliverables) oss << "\t" << deliverable.to_string() << "\n";
        oss << "}\n"
            << "Next Page Token: " << next_page_token << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["symbol"] = symbol;
        j["name"] = name;
        j["status"] = status;
        j["tradable"] = tradable;
        j["expiration_date"] = expiration_date.to_string();
        j["root_symbol"] = root_symbol;
        j["underlying_symbol"] = underlying_symbol;
        j["underlying_asset_id"] = underlying_asset_id;
        j["type"] = type;
        j["style"] = style;
        j["strike_price"] = strike_price;
        j["multiplier"] = multiplier;
        j["size"] = size;
        j["open_interest"] = open_interest;
        j["open_interest_date"] = open_interest_date.to_string();
        j["close_price"] = close_price;
        j["close_price_date"] = close_price_date.to_string();
        
        for (const auto& deliverable : deliverables) {
            j["deliverables"].push_back(deliverable.to_json());
        }
        
        return j;
    }

};


} // namespace alpaca