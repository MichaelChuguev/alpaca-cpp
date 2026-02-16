#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/model/trader/order.h"

namespace alpaca {

using json = nlohmann::json;

struct Position {

    const std::string asset_id;
    const std::string symbol;
    const Exchange exchange;
    const AssetClass asset_class;
    const double avg_entry_price;
    const double qty;
    const double qty_available;
    const OrderSide side;
    const double market_value;
    const double cost_basis;
    const double unrealized_pl;
    const double unrealized_plpc;
    const double unrealized_intraday_pl;
    const double unrealized_intraday_plpc;
    const double current_price;
    const double lastday_price;
    const double change_today;
    const bool asset_marginable;

    Position(const json& j)
    :
        asset_id(j["asset_id"].get<std::string>()),
        symbol(j["symbol"].get<std::string>()),
        exchange(j["exchange"].get<std::string>() == "AMEX" ? Exchange::AMEX : 
                 j["exchange"].get<std::string>() == "NYSE" ? Exchange::NYSE :
                 j["exchange"].get<std::string>() == "NASDAQ" ? Exchange::NASDAQ :
                 j["exchange"].get<std::string>() == "BATS" ? Exchange::BATS :
                 j["exchange"].get<std::string>() == "NYSEARCA" ? Exchange::NYSEARCA :
                 j["exchange"].get<std::string>() == "OTC" ? Exchange::OTC : Exchange::NYSE),
        asset_class(j["asset_class"].get<std::string>() == "us_equity" ? AssetClass::US_EQUITY :
                    j["asset_class"].get<std::string>() == "us_option" ? AssetClass::US_OPTION :
                    j["asset_class"].get<std::string>() == "crypto" ? AssetClass::CRYPTO : AssetClass::US_EQUITY),
        avg_entry_price(j["avg_entry_price"].get<double>()),
        qty(j["qty"].get<double>()),
        qty_available(j["qty_available"].get<double>()),
        side(j["side"].get<std::string>() == "buy" ? OrderSide::BUY : OrderSide::SELL),
        market_value(j["market_value"].get<double>()),
        cost_basis(j["cost_basis"].get<double>()),
        unrealized_pl(j["unrealized_pl"].get<double>()),
        unrealized_plpc(j["unrealized_plpc"].get<double>()),
        unrealized_intraday_pl(j["unrealized_intraday_pl"].get<double>()),
        unrealized_intraday_plpc(j["unrealized_intraday_plpc"].get<double>()),
        current_price(j["current_price"].get<double>()),
        lastday_price(j["lastday_price"].get<double>()),
        change_today(j["change_today"].get<double>()),
        asset_marginable(j["asset_marginable"].get<bool>())
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Position: " << symbol << "\n" 
            << "Asset ID: " << asset_id << "\n"
            << "Exchange: " << (exchange == Exchange::AMEX ? "AMEX" : 
                              exchange == Exchange::NYSE ? "NYSE" : 
                              exchange == Exchange::NASDAQ ? "NASDAQ" : 
                              exchange == Exchange::BATS ? "BATS" : 
                              exchange == Exchange::NYSEARCA ? "NYSEARCA" : 
                              exchange == Exchange::OTC ? "OTC" : "UNKNOWN") << "\n"
            << "Asset Class: " << (asset_class == AssetClass::US_EQUITY ? "US_EQUITY" : 
                                 asset_class == AssetClass::US_OPTION ? "US_OPTION" : 
                                 asset_class == AssetClass::CRYPTO ? "CRYPTO" : "UNKNOWN") << "\n"
            << "Avg Entry Price: " << avg_entry_price << "\n"
            << "Qty: " << qty << "\n"
            << "Qty Available: " << qty_available << "\n"
            << "Side: " << (side == OrderSide::BUY ? "BUY" : "SELL") << "\n"
            << "Market Value: " << market_value << "\n"
            << "Cost Basis: " << cost_basis << "\n"
            << "Unrealized P/L: " << unrealized_pl << "\n"
            << "Unrealized P/L %: " << unrealized_plpc << "%\n"
            << "Unrealized Intraday P/L: " << unrealized_intraday_pl  << "\n"
            << "Unrealized Intraday P/L %: "  << unrealized_intraday_plpc << "%\n"
            << "Current Price: $"  << current_price  << "\n"
            << "Last Day Price: $"  << lastday_price  << "\n"
            << "Change Today: "  << change_today << "%\n"
            << "Asset Marginable: "  << (asset_marginable ? "true" : "false") << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["asset_id"] = asset_id;
        j["symbol"] = symbol;
        j["exchange"] = (exchange == Exchange::AMEX ? "AMEX" : 
                         exchange == Exchange::NYSE ? "NYSE" : 
                         exchange == Exchange::NASDAQ ? "NASDAQ" : 
                         exchange == Exchange::BATS ? "BATS" : 
                         exchange == Exchange::NYSEARCA ? "NYSEARCA" : 
                         exchange == Exchange::OTC ? "OTC" : "UNKNOWN");
        j["asset_class"] = (asset_class == AssetClass::US_EQUITY ? "us_equity" : 
                            asset_class == AssetClass::US_OPTION ? "us_option" : 
                            asset_class == AssetClass::CRYPTO ? "crypto" : "unknown");
        j["avg_entry_price"] = avg_entry_price;
        j["qty"] = qty;
        j["qty_available"] = qty_available;
        j["side"] = (side == OrderSide::BUY ? "buy" : "sell");
        j["market_value"] = market_value;
        j["cost_basis"] = cost_basis;
        j["unrealized_pl"] = unrealized_pl;
        j["unrealized_plpc"] = unrealized_plpc;
        j["unrealized_intraday_pl"] = unrealized_intraday_pl;
        j["unrealized_intraday_plpc"] = unrealized_intraday_plpc;
        j["current_price"] = current_price;
        j["lastday_price"] = lastday_price;
        j["change_today"] = change_today;
        j["asset_marginable"] = asset_marginable;

        return j;
    }

};

struct PositionClosed {

    const std::string symbol;
    const int status;
    const OrderResponse body;

    PositionClosed(const json& j)
    :
        symbol(j["symbol"].get<std::string>()),
        status(j["status"].get<int>()),
        body(OrderResponse(j["body"]))
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "PositionClosed: " << symbol << "\n" 
            << "Status: " << status << "\n"
            << body.to_string() << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["symbol"] = symbol;
        j["status"] = status;
        j["body"] = body.to_json();
        return j;
    }

};


} // namespace alpaca