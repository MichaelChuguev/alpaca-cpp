#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

struct Leg {

    std::string symbol;
    OrderSide side;
    double ratio_qty;
    PositionIntent position_intent;

    Leg(const std::string& symbol, OrderSide side, double ratio_qty, PositionIntent position_intent)
    : 
        symbol(symbol),
        side(side),
        ratio_qty(ratio_qty),
        position_intent(position_intent)
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Leg: " << symbol << "\n" 
            << "Side: " << (side == BUY ? "BUY" : "SELL") << "\n"
            << "Ratio Qty: " << ratio_qty << "\n"
            << "Position Intent: " << (position_intent == BUY_TO_OPEN ? "BUY_TO_OPEN" : 
                                         position_intent == BUY_TO_CLOSE ? "BUY_TO_CLOSE" : 
                                         position_intent == SELL_TO_OPEN ? "SELL_TO_OPEN" : 
                                         "SELL_TO_CLOSE") << "\n";

        return oss.str();
    }

    json to_json() const {
        json j;
        j["symbol"] = symbol;
        j["side"] = (side == BUY ? "BUY" : "SELL");
        j["ratio_qty"] = ratio_qty;
        j["position_intent"] = (position_intent == BUY_TO_OPEN ? "BUY_TO_OPEN" : 
                                position_intent == BUY_TO_CLOSE ? "BUY_TO_CLOSE" : 
                                position_intent == SELL_TO_OPEN ? "SELL_TO_OPEN" : 
                                "SELL_TO_CLOSE");
        return j;
    }

};

struct Order {

    std::string symbol;
    double qty;
    double notional;
    OrderSide side;
    OrderType type;
    OrderTimeInForce time_in_force;
    double limit_price;
    double stop_price;
    double trail_price;
    double trail_percent;
    bool extended_hours;
    std::string client_order_id;
    OrderClass order_class;
    std::vector<Leg> legs;
    std::map<std::string, double> take_profit;
    std::map<std::string, double> stop_loss;
    PositionIntent position_intent;

    public:

    // Default constructor
    Order() = default;

    /**
     * 
     */
    Order(const std::string& symbol, const double& qty, const OrderSide& side, const OrderType& type, const OrderTimeInForce& time_in_force)
    : symbol(symbol), qty(qty), notional(0.0), side(side), type(type), time_in_force(time_in_force),
      limit_price(0.0), stop_price(0.0), trail_price(0.0), trail_percent(0.0),
      extended_hours(false), order_class(OrderClass::SIMPLE), position_intent(PositionIntent::BUY_TO_OPEN)
    {}


    std::string to_string() const {
        std::ostringstream oss;
        oss << "Order: " << symbol << "\n" 
            << "Qty: " << qty << "\n"
            << "Notional: " << notional << "\n"
            << "Side: " << (side == BUY ? "BUY" : "SELL") << "\n"
            << "Type: " << (type == MARKET ? "MARKET" : 
                          type == LIMIT ? "LIMIT" : 
                          type == STOP ? "STOP" : 
                          type == STOP_LIMIT ? "STOP_LIMIT" : 
                          type == TRAILING_STOP ? "TRAILING_STOP" : "UNKNOWN") << "\n"
            << "Time In Force: " << (time_in_force == DAY ? "DAY" : 
                                    time_in_force == GTC ? "GTC" : 
                                    time_in_force == OPG ? "OPG" : 
                                    time_in_force == CLS ? "CLS" : 
                                    time_in_force == IOC ? "IOC" : 
                                    time_in_force == FOK ? "FOK" : "UNKNOWN") << "\n"
            << "Limit Price: " << limit_price << "\n"
            << "Stop Price: " << stop_price << "\n"
            << "Trail Price: " << trail_price << "\n"
            << "Trail Percent: " << trail_percent << "\n"
            << "Extended Hours: " << (extended_hours ? "true" : "false") << "\n"
            << "Client Order ID: " << client_order_id << "\n"
            << "Order Class: " << (order_class == SIMPLE ? "SIMPLE" : 
                                  order_class == OCO ? "OCO" : 
                                  order_class == BRACKET ? "BRACKET" : 
                                  order_class == MLEG ? "MLEG" : 
                                  order_class == OTO ? "OTO" : "UNKNOWN") << "\n"
            << "Legs: \n";
            for (const auto& leg : legs) oss << leg.to_string() << "\n";
            oss << "Take Profit: \n";
            for (const auto& [key, value] : take_profit) oss << key << ": " << value << "\n";
            oss << "Stop Loss: \n";
            for (const auto& [key, value] : stop_loss) oss << key << ": " << value << "\n";
            oss << "Position Intent: " << (position_intent == BUY_TO_OPEN ? "BUY_TO_OPEN" : 
                                        position_intent == BUY_TO_CLOSE ? "BUY_TO_CLOSE" : 
                                        position_intent == SELL_TO_OPEN ? "SELL_TO_OPEN" : 
                                        position_intent == SELL_TO_CLOSE ? "SELL_TO_CLOSE" : "UNKNOWN") << "\n";

        return oss.str();
    }

    json to_json() const {

        Order::validate();
    
        json j;
        j["symbol"] = symbol;
        
        if (qty > 0) {
            j["qty"] = qty;
        } else if (notional > 0) {
            j["notional"] = notional;
        }
        
        j["side"] = order_side_to_string(side);
        j["type"] = order_type_to_string(type);
        j["time_in_force"] = time_in_force_to_string(time_in_force);
    
        if (limit_price > 0) {
            j["limit_price"] = std::to_string(limit_price);
        }
        if (stop_price > 0) {
            j["stop_price"] = std::to_string(stop_price);
        }
        if (trail_price > 0) {
            j["trail_price"] = std::to_string(trail_price);
        }
        if (trail_percent > 0) {
            j["trail_percent"] = std::to_string(trail_percent);
        }
        
        if (extended_hours) {
            j["extended_hours"] = extended_hours;
        }
        if (!client_order_id.empty()) {
            j["client_order_id"] = client_order_id;
        }
        
        if (order_class != SIMPLE) {
            j["order_class"] = order_class_to_string(order_class);
        }
        
        if (!legs.empty()) {
            j["legs"] = json::array();
            for (const auto& leg : legs) {
                json leg_json;
                leg_json["symbol"] = leg.symbol;
                leg_json["side"] = order_side_to_string(leg.side);
                leg_json["ratio_qty"] = leg.ratio_qty;
                j["legs"].push_back(leg_json);
            }
        }
        
        if (!take_profit.empty()) {
            j["take_profit"] = json::object();
            for (const auto& [key, value] : take_profit) {
                j["take_profit"][key] = std::to_string(value);
            }
        }
        if (!stop_loss.empty()) {
            j["stop_loss"] = json::object();
            for (const auto& [key, value] : stop_loss) {
                j["stop_loss"][key] = std::to_string(value);
            }
        }
        return j;
    }

    private:

    /**
     * Validates the order parameters to make sure they match API rules prior to being sent. Variables marked 
     * with 'REQUIRED' must always be present in the order, no matter what configurations exist. Others may or 
     * may not be required depending on the type of order or desired order features.
     * 
     * The RULES for order parameters are listed below - if not followed, the Alpaca API will not accept 
     * the order:
     * 
     * @note 1. symbol - symbol, asset ID, or currency pair to identify the asset to trade, 
     *          required for all order classes except for mleg.
     * 
     * @note 2. qty - number of shares to trade. Can be fractionable for only market and day order types. 
     *          Required for mleg order class, represents the number of units to trade of this strategy.
     * 
     * @note 3. notional - dollar amount to trade. Cannot work with qty. Can only work for market order types 
     *          and day for time in force.
     * 
     * @note 4. side - Represents which side this order was on: buy, sell. Required for all order classes 
     *          except for mleg.
     * 
     * @note 5. REQUIRED - type - The order types supported by Alpaca vary based on the order's security type. 
     *          The following provides a comprehensive breakdown of the supported order types for each category:
     *
     *          - Equity trading: market, limit, stop, stop_limit, trailing_stop.
     *          - Options trading: market, limit, stop, stop_limit.
     *          - Multileg Options trading: market, limit.
     *          - Crypto trading: market, limit, stop_limit.
     * 
     * @note 6. REQUIRED - time_in_force - The Time-In-Force values supported by Alpaca vary based on the order's 
     *          security type. Here is a breakdown of the supported TIFs for each specific security type:
     * 
     *          - Equity trading: day, gtc, opg, cls, ioc, fok.
     *          - Options trading: day.
     *          - Crypto trading: gtc, ioc.
     * 
     * @note 7. limit_price - Required if type is limit or stop_limit. In case of mleg, the limit_price 
     *          parameter is expressed with the following notation:
     * 
     *          - A positive value indicates a debit, representing a cost or payment to be made.
     *          - A negative value signifies a credit, reflecting an amount to be received.
     * 
     * @note 8. stop_price - required if type is stop or stop_limit.
     * 
     * @note 9. trail_price - this or trail_percent is required if type is trailing_stop.
     * 
     * @note 10. trail_percent - this or trail_price is required if type is trailing_stop.
     * 
     * @note 11. extended_hours - (default) false. If true, order will be eligible to execute in 
     *           premarket/afterhours. Only works with type limit and time_in_force day.
     * 
     * @note 12. client_order_id - A unique identifier for the order. Automatically generated if not sent. 
     *           (<= 128 characters).
     * 
     * @note 13. order_class - The order classes supported by Alpaca vary based on the order's security type. 
     *           The following provides a comprehensive breakdown of the supported order classes for each 
     *           category:
     * 
     *           - Equity trading: simple (or ""), oco, oto, bracket.
     *           - Options trading: simple (or ""), mleg (required for multi-leg complex option strategies).
     *           - Crypto trading: simple (or "").
     * 
     * @note 14. legs - list of order legs (<= 4).
     *                - side - Represents which side this order was on: buy, sell. Required for all order 
     *                         classes except for mleg.
     *                - position_intent - Represents the desired position strategy.
     *                - REQUIRED - symbol - symbol or asset ID to identify the asset to trade.
     *                - REQUIRED - ratio_qty - proportional quantity of this leg in relation to the overall multi-leg 
     *                              order qty.
     * 
     * @note 15. take_profit - Additional parameters for take-profit leg of advanced orders.
     *                       - key ("limit_price").
     *                       - value (price).
     * 
     * @note 16. stop_loss - Additional parameters for stop-loss leg of advanced orders.
     *                     - key ("stop_price", "limit_price").
     *                     - value (price).
     * 
     * @note 17. position_intent - Represents the desired position strategy.
     */
    void validate() const {
    
        // Rule 1: Symbol required for all order classes except MLEG
        if (order_class != MLEG && symbol.empty()) {
            throw std::invalid_argument("Symbol is required for non-MLEG orders");
        }
    
        // Rule 2/3: Either qty or notional must be set (but not both)
        const bool has_qty = qty > 0;
        const bool has_notional = notional > 0;
        
        if (!has_qty && !has_notional) {
            throw std::invalid_argument("Either qty or notional must be specified");
        }
        
        if (has_qty && has_notional) {
            throw std::invalid_argument("Cannot specify both qty and notional");
        }
    
        // Rule 4: Side required for non-MLEG orders
        if (order_class != MLEG && (side != BUY && side != SELL)) {
            throw std::invalid_argument("Valid side (BUY/SELL) is required for non-MLEG orders");
        }
    
        // Rule 5: Type validation
        if (type < MARKET || type > TRAILING_STOP) {
            throw std::invalid_argument("Invalid order type");
        }
    
        // Rule 6: Time in force validation
        if (time_in_force < DAY || time_in_force > FOK) {
            throw std::invalid_argument("Invalid time in force");
        }
    
        // Rule 7: Limit price required for limit/stop_limit orders
        if ((type == LIMIT || type == STOP_LIMIT) && limit_price <= 0) {
            throw std::invalid_argument("Limit price required for limit/stop_limit orders");
        }
    
        // Rule 8: Stop price required for stop/stop_limit orders
        if ((type == STOP || type == STOP_LIMIT) && stop_price <= 0) {
            throw std::invalid_argument("Stop price required for stop/stop_limit orders");
        }
    
        // Rule 9/10: Trail price or percent required for trailing stop
        if (type == TRAILING_STOP && trail_price <= 0 && trail_percent <= 0) {
            throw std::invalid_argument("Either trail_price or trail_percent required for trailing_stop orders");
        }
    
        // Rule 11: Extended hours validation
        if (extended_hours && (type != LIMIT || time_in_force != DAY)) {
            throw std::invalid_argument("Extended hours only allowed for limit orders with DAY time_in_force");
        }
    
        // Rule 12: Client order ID length check
        if (!client_order_id.empty() && client_order_id.length() > 128) {
            throw std::invalid_argument("Client order ID must be <= 128 characters");
        }
    
        // Rule 13: Order class validation
        if (order_class < SIMPLE || order_class > MLEG) {
            throw std::invalid_argument("Invalid order class");
        }
    
        // Rule 14: Legs validation
        if (!legs.empty()) {
            if (legs.size() > 4) {
                throw std::invalid_argument("Maximum 4 legs allowed");
            }
    
            for (const auto& leg : legs) {
                if (leg.symbol.empty()) {
                    throw std::invalid_argument("Leg symbol is required");
                }
                if (leg.ratio_qty <= 0) {
                    throw std::invalid_argument("Leg ratio_qty must be positive");
                }
            }
        }
    
        // Rule 15/16: Advanced order parameters validation
        if (order_class == BRACKET || order_class == OCO || order_class == OTO) {
            if (take_profit.empty() && stop_loss.empty()) {
                throw std::invalid_argument("Advanced orders require take_profit or stop_loss parameters");
            }
    
            if (!take_profit.empty() && take_profit.find("limit_price") == take_profit.end()) {
                throw std::invalid_argument("take_profit requires limit_price");
            }
    
            if (!stop_loss.empty()) {
                if (stop_loss.find("stop_price") == stop_loss.end()) {
                    throw std::invalid_argument("stop_loss requires stop_price");
                }
                // For stop-limit orders
                if (type == STOP_LIMIT && stop_loss.find("limit_price") == stop_loss.end()) {
                    throw std::invalid_argument("stop_loss requires limit_price for stop-limit orders");
                }
            }
        }
    
        // Rule 17: Position intent validation
        if (position_intent < BUY_TO_OPEN || position_intent > SELL_TO_CLOSE) {
            throw std::invalid_argument("Invalid position intent");
        }
    
        // Additional validations based on security type would go here
    }

};

enum OrderStatus {
    OPEN,
    CLOSED,
    ALL
};

struct OrderResponse {

    const std::string id;
    const std::string client_order_id;
    const DateTime created_at;
    const DateTime updated_at;
    const DateTime submitted_at;
    const DateTime filled_at;
    const DateTime expired_at;
    const DateTime canceled_at;
    const DateTime failed_at;
    const DateTime replaced_at;
    const std::string replaced_by;
    const std::string replaces;
    const std::string asset_id;
    const std::string symbol;
    const AssetClass asset_class;
    const double notional;
    const double qty;
    const double filled_qty;
    const double filled_avg_price;
    const OrderClass order_class;
    const OrderType type;
    const OrderSide side;
    const PositionIntent position_intent;
    const OrderTimeInForce time_in_force;
    const double limit_price;
    const double stop_price;
    const std::string status;
    const bool extended_hours;
    mutable std::vector<OrderResponse> legs;
    const double trail_percent;
    const double trail_price;
    const double hwm;

    OrderResponse(const json& j)
    : 
        id(parse_string(j, "id")),
        client_order_id(parse_string(j, "client_order_id")),
        created_at(parse_datetime(j, "created_at")),
        updated_at(parse_datetime(j, "updated_at")),
        submitted_at(parse_datetime(j, "submitted_at")),
        filled_at(parse_datetime(j, "filled_at")),
        expired_at(parse_datetime(j, "expired_at")),
        canceled_at(parse_datetime(j, "canceled_at")),
        failed_at(parse_datetime(j, "failed_at")),
        replaced_at(parse_datetime(j, "replaced_at")),
        replaced_by(parse_string(j, "replaced_by")),
        replaces(parse_string(j, "replaces")),
        asset_id(parse_string(j, "asset_id")),
        symbol(parse_string(j, "symbol")),
        asset_class(string_to_asset_class(parse_string(j, "asset_class"))),
        notional(parse_decimal(j, "notional")),
        qty(parse_decimal(j, "qty")),
        filled_qty(parse_decimal(j, "filled_qty")),
        filled_avg_price(parse_decimal(j, "filled_avg_price")),
        order_class(string_to_order_class(parse_string(j, "order_class"))),
        type(string_to_order_type(parse_string(j, "type"))),
        side(string_to_order_side(parse_string(j, "side"))),
        position_intent(string_to_position_intent(parse_string(j, "position_intent"))),
        time_in_force(string_to_time_in_force(parse_string(j, "time_in_force"))),
        limit_price(parse_decimal(j, "limit_price")),
        stop_price(parse_decimal(j, "stop_price")),
        status(parse_string(j, "status")),
        extended_hours(parse_bool(j, "extended_hours")),
        trail_percent(parse_decimal(j, "trail_percent")),
        trail_price(parse_decimal(j, "trail_price")),
        hwm(parse_decimal(j, "hwm"))
    {
        if (j.contains("legs") && !j["legs"].is_null()) {
            for (const auto& leg : j["legs"]) {
                legs.emplace_back(leg);
            }
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Order Status:\n"
            << "ID: " << id << "\n"
            << "Client Order ID: " << client_order_id << "\n"
            << "Created At: " << created_at.to_string() << "\n"
            << "Updated At: " << updated_at.to_string() << "\n"
            << "Submitted At: " << submitted_at.to_string() << "\n"
            << "Filled At: " << filled_at.to_string() << "\n"
            << "Expired At: " << expired_at.to_string() << "\n"
            << "Canceled At: " << canceled_at.to_string() << "\n"
            << "Failed At: " << failed_at.to_string() << "\n"
            << "Replaced At: " << replaced_at.to_string() << "\n"
            << "Replaced By: " << replaced_by << "\n"
            << "Replaces: " << replaces << "\n"
            << "Asset ID: " << asset_id << "\n"
            << "Symbol: " << symbol << "\n"
            << "Asset Class: " << asset_class << "\n"
            << "Notional: " << notional << "\n"
            << "Quantity: " << qty << "\n"
            << "Filled Quantity: " << filled_qty << "\n"
            << "Filled Average Price: " << filled_avg_price << "\n"
            << "Order Class: " << order_class << "\n"
            << "Order Type: " << type << "\n"
            << "Order Side: " << side << "\n"
            << "Position Intent: " << position_intent << "\n"
            << "Time In Force: " << time_in_force << "\n"
            << "Limit Price: " << limit_price << "\n"
            << "Stop Price: " << stop_price << "\n"
            << "Order Status: " << status << "\n"
            << "Extended Hours: " << extended_hours << "\n"
            << "Legs: \n";
            for (const auto& leg : legs) oss << leg.to_string() << "\n";
        oss << "Trail Percent: " << trail_percent << "\n"
            << "Trail Price: " << trail_price << "\n"
            << "HWM: " << hwm << "\n";

        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["client_order_id"] = client_order_id;
        j["created_at"] = created_at.to_string();
        j["updated_at"] = updated_at.to_string();
        j["submitted_at"] = submitted_at.to_string();
        j["filled_at"] = filled_at.to_string();
        j["expired_at"] = expired_at.to_string();
        j["canceled_at"] = canceled_at.to_string();
        j["failed_at"] = failed_at.to_string();
        j["replaced_at"] = replaced_at.to_string();
        j["replaced_by"] = replaced_by;
        j["replaces"] = replaces;
        j["asset_id"] = asset_id;
        j["symbol"] = symbol;
        j["asset_class"] = asset_class_to_string(asset_class);
        j["notional"] = notional;
        j["qty"] = qty;
        j["filled_qty"] = filled_qty;
        j["filled_avg_price"] = filled_avg_price;
        j["order_class"] = order_class_to_string(order_class);
        j["type"] = order_type_to_string(type);
        j["side"] = order_side_to_string(side);
        j["position_intent"] = position_intent_to_string(position_intent);
        j["time_in_force"] = time_in_force_to_string(time_in_force);
        j["limit_price"] = limit_price;
        j["stop_price"] = stop_price;
        j["status"] = status;
        j["extended_hours"] = extended_hours;
        j["legs"] = json::array();
        for (const auto& leg : legs) {
            j["legs"].push_back(leg.to_json());
        }
        j["trail_percent"] = trail_percent;
        j["trail_price"] = trail_price;
        j["hwm"] = hwm;
        return j;
    }


};


} // namespace alpaca