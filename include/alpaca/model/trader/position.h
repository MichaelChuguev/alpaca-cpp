#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/core/util.h"
#include "alpaca/model/trader/order.h"

namespace alpaca {

using json = nlohmann::json;

struct Position {

    std::string asset_id;
    std::string symbol;
    Exchange exchange;
    AssetClass asset_class;
    Decimal avg_entry_price;
    Decimal qty;
    Decimal qty_available;
    OrderSide side;
    Decimal market_value;
    Decimal cost_basis;
    Decimal unrealized_pl;
    Decimal unrealized_plpc;
    Decimal unrealized_intraday_pl;
    Decimal unrealized_intraday_plpc;
    Decimal current_price;
    Decimal lastday_price;
    Decimal change_today;
    bool asset_marginable;

    Position(const json& j)
    :
        asset_id(j["asset_id"].get<std::string>()),
        symbol(j["symbol"].get<std::string>()),
        exchange(string_to_exchange(j["exchange"].get<std::string>())),
        asset_class(string_to_asset_class(j["asset_class"].get<std::string>())),
        avg_entry_price(parse_decimal(j, "avg_entry_price")),
        qty(parse_decimal(j, "qty")),
        qty_available(parse_decimal(j, "qty_available")),
        side(string_to_order_side(j["side"].get<std::string>())),
        market_value(parse_decimal(j, "market_value")),
        cost_basis(parse_decimal(j, "cost_basis")),
        unrealized_pl(parse_decimal(j, "unrealized_pl")),
        unrealized_plpc(parse_decimal(j, "unrealized_plpc")),
        unrealized_intraday_pl(parse_decimal(j, "unrealized_intraday_pl")),
        unrealized_intraday_plpc(parse_decimal(j, "unrealized_intraday_plpc")),
        current_price(parse_decimal(j, "current_price")),
        lastday_price(parse_decimal(j, "lastday_price")),
        change_today(parse_decimal(j, "change_today")),
        asset_marginable(j["asset_marginable"].get<bool>())
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Position: " << symbol << "\n" 
            << "Asset ID: " << asset_id << "\n"
            << "Exchange: " << exchange_to_string(exchange) << "\n"
            << "Asset Class: " << asset_class_to_string(asset_class) << "\n"
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
        j["exchange"] = exchange_to_string(exchange);
        j["asset_class"] = asset_class_to_string(asset_class);
        j["avg_entry_price"] = avg_entry_price.to_double();
        j["qty"] = qty.to_double();
        j["qty_available"] = qty_available.to_double();
        j["side"] = order_side_to_string(side);
        j["market_value"] = market_value.to_double();
        j["cost_basis"] = cost_basis.to_double();
        j["unrealized_pl"] = unrealized_pl.to_double();
        j["unrealized_plpc"] = unrealized_plpc.to_double();
        j["unrealized_intraday_pl"] = unrealized_intraday_pl.to_double();
        j["unrealized_intraday_plpc"] = unrealized_intraday_plpc.to_double();
        j["current_price"] = current_price.to_double();
        j["lastday_price"] = lastday_price.to_double();
        j["change_today"] = change_today.to_double();
        j["asset_marginable"] = asset_marginable;

        return j;
    }

};

struct PositionClosed {

    std::string symbol;
    int status;
    OrderResponse body;

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