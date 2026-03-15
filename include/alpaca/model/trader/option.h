#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

enum class OptionContractType {
    CALL,
    PUT
};

inline std::string option_contract_type_to_string(OptionContractType t) {
    switch (t) {
        case OptionContractType::CALL: return "call";
        case OptionContractType::PUT:  return "put";
        default: return "";
    }
}

inline OptionContractType string_to_option_contract_type(const std::string& s) {
    if (s == "call") return OptionContractType::CALL;
    if (s == "put")  return OptionContractType::PUT;
    return OptionContractType::CALL;
}

enum class OptionContractStyle {
    AMERICAN,
    EUROPEAN
};

inline std::string option_contract_style_to_string(OptionContractStyle s) {
    switch (s) {
        case OptionContractStyle::AMERICAN: return "american";
        case OptionContractStyle::EUROPEAN: return "european";
        default: return "";
    }
}

inline OptionContractStyle string_to_option_contract_style(const std::string& s) {
    if (s == "american") return OptionContractStyle::AMERICAN;
    if (s == "european") return OptionContractStyle::EUROPEAN;
    return OptionContractStyle::AMERICAN;
}

enum class OptionContractStatus {
    ACTIVE,
    INACTIVE
};

inline std::string option_contract_status_to_string(OptionContractStatus s) {
    switch (s) {
        case OptionContractStatus::ACTIVE:   return "active";
        case OptionContractStatus::INACTIVE: return "inactive";
        default: return "";
    }
}

inline OptionContractStatus string_to_option_contract_status(const std::string& s) {
    if (s == "active")   return OptionContractStatus::ACTIVE;
    if (s == "inactive") return OptionContractStatus::INACTIVE;
    return OptionContractStatus::ACTIVE;
}

struct OptionContract {

    struct Deliverable {
        std::string type;
        std::string symbol;
        std::string asset_id;
        Decimal amount;
        Decimal allocation_percentage;
        std::string settlement_type;
        std::string settlement_method;
        bool delayed_settlement;

        Deliverable(const json& j) 
        :
            type(j["type"]),
            symbol(j["symbol"]),
            asset_id(j["asset_id"]),
            amount(Decimal(j["amount"].get<std::string>())),
            allocation_percentage(Decimal(j["allocation_percentage"].get<std::string>())),
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
            j["amount"] = amount.to_double();
            j["allocation_percentage"] = allocation_percentage.to_double();
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
    Decimal strike_price;
    Decimal multiplier;
    Decimal size;
    Decimal open_interest;
    DateTime open_interest_date;
    Decimal close_price;
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
        strike_price(j.contains("strike_price") ? Decimal(j["strike_price"].get<std::string>()) : Decimal{}),
        multiplier(j.contains("multiplier") ? Decimal(j["multiplier"].get<std::string>()) : Decimal{}),
        size(j.contains("size") ? Decimal(j["size"].get<std::string>()) : Decimal{}),
        open_interest(j.contains("open_interest") ? Decimal(j["open_interest"].get<std::string>()) : Decimal{}),
        open_interest_date(DateTime::parse(j.value("open_interest_date", ""))),
        close_price(j.contains("close_price") ? Decimal(j["close_price"].get<std::string>()) : Decimal{}),
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
        j["strike_price"] = strike_price.to_double();
        j["multiplier"] = multiplier.to_double();
        j["size"] = size.to_double();
        j["open_interest"] = open_interest.to_double();
        j["open_interest_date"] = open_interest_date.to_string();
        j["close_price"] = close_price.to_double();
        j["close_price_date"] = close_price_date.to_string();
        
        for (const auto& deliverable : deliverables) {
            j["deliverables"].push_back(deliverable.to_json());
        }
        
        return j;
    }

};


} // namespace alpaca