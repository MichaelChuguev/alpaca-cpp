#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

enum OrderSide {
    BUY,
    SELL
};

enum OrderType {
    MARKET,
    LIMIT, 
    STOP,
    STOP_LIMIT,
    TRAILING_STOP
};

enum OrderTimeInForce {
    DAY, // Day Order
    GTC, // Good Till Cancelled
    OPG, 
    CLS, 
    IOC, // Immediate Or Cancel
    FOK, // Fill Or Kill 
};

enum OrderClass {
    SIMPLE, 
    BRACKET, 
    OCO, // One Cancels Other
    OTO, // One Triggers Other
    MLEG // Multi-Leg
};

enum PositionIntent {
    BUY_TO_OPEN,
    BUY_TO_CLOSE,
    SELL_TO_OPEN,
    SELL_TO_CLOSE
};

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
    Order(const std::string& symbol, const double& qty, const OrderSide& side, const OrderType& type, const OrderTimeInForce& time_in_force);

    //TODO MORE ORDER CONSTRUCTS




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
        j["qty"] = std::to_string(qty);
        j["notional"] = std::to_string(notional);
        j["side"] = (side == BUY ? "buy" : "sell");
        j["type"] = (type == MARKET ? "market" : 
                     type == LIMIT ? "limit" : 
                     type == STOP ? "stop" : 
                     type == STOP_LIMIT ? "stop_limit" : 
                     type == TRAILING_STOP ? "trailing_stop" : "unknown");
        j["time_in_force"] = (time_in_force == DAY ? "day" : 
                                time_in_force == GTC ? "gtc" : 
                                time_in_force == OPG ? "opg" : 
                                time_in_force == CLS ? "cls" : 
                                time_in_force == IOC ? "ioc" : 
                                time_in_force == FOK ? "fok" : "unknown");
    
        j["limit_price"] = std::to_string(limit_price);
        j["stop_price"] = std::to_string(stop_price);
        j["trail_price"] = std::to_string(trail_price);
        j["trail_percent"] = std::to_string(trail_percent);
        j["extended_hours"] = extended_hours;
        j["client_order_id"] = client_order_id;
        j["order_class"] = (order_class == SIMPLE ? "simple" : 
                            order_class == OCO ? "oco" : 
                            order_class == BRACKET ? "bracket" : 
                            order_class == MLEG ? "mleg" : 
                            order_class == OTO ? "oto" : "unknown");
        j["position_intent"] = (position_intent == BUY_TO_OPEN ? "buy_to_open" : 
                                position_intent == BUY_TO_CLOSE ? "buy_to_close" : 
                                position_intent == SELL_TO_OPEN ? "sell_to_open" : 
                                position_intent == SELL_TO_CLOSE ? "sell_to_close" : "unknown");
        j["legs"] = json::array();
        for (const auto& leg : legs) {
            json leg_json;
            leg_json["symbol"] = leg.symbol;
            leg_json["side"] = (leg.side == BUY ? "buy" : "sell");
            leg_json["ratio_qty"] = std::to_string(leg.ratio_qty);
            leg_json["position_intent"] = (leg.position_intent == BUY_TO_OPEN ? "buy_to_open" : 
                                           leg.position_intent == BUY_TO_CLOSE ? "buy_to_close" : 
                                           leg.position_intent == SELL_TO_OPEN ? "sell_to_open" : 
                                           leg.position_intent == SELL_TO_CLOSE ? "sell_to_close" : "unknown");
            j["legs"].push_back(leg_json);
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
        id(j["id"].get<std::string>()),
        client_order_id(j["client_order_id"].get<std::string>()),
        created_at(DateTime::parse(j["created_at"].get<std::string>())),
        updated_at(DateTime::parse(j["updated_at"].get<std::string>())),
        submitted_at(DateTime::parse(j["submitted_at"].get<std::string>())),
        filled_at(DateTime::parse(j["filled_at"].get<std::string>())),
        expired_at(DateTime::parse(j["expired_at"].get<std::string>())),
        canceled_at(DateTime::parse(j["canceled_at"].get<std::string>())),
        failed_at(DateTime::parse(j["failed_at"].get<std::string>())),
        replaced_at(DateTime::parse(j["replaced_at"].get<std::string>())),
        replaced_by(j["replaced_by"].get<std::string>()),
        replaces(j["replaces"].get<std::string>()),
        asset_id(j["asset_id"].get<std::string>()),
        symbol(j["symbol"].get<std::string>()),
        asset_class(j["asset_class"].get<std::string>() == "us_equity" ? US_EQUITY : 
                    j["asset_class"].get<std::string>() == "us_option" ? US_OPTION : CRYPTO),
        notional(j["notional"].get<double>()),
        qty(j["qty"].get<double>()),
        filled_qty(j["filled_qty"].get<double>()),
        filled_avg_price(j["filled_avg_price"].get<double>()),
        order_class(j["order_class"].get<std::string>() == "simple" ? SIMPLE : 
                    j["order_class"].get<std::string>() == "oco" ? OCO : 
                    j["order_class"].get<std::string>() == "bracket" ? BRACKET : 
                    j["order_class"].get<std::string>() == "mleg" ? MLEG : OTO),
        type(j["type"].get<std::string>() == "market" ? MARKET :
                    j["type"].get<std::string>() == "limit" ? LIMIT : 
                    j["type"].get<std::string>() == "stop" ? STOP : 
                    j["type"].get<std::string>() == "stop_limit" ? STOP_LIMIT : 
                    j["type"].get<std::string>() == "trailing_stop" ? TRAILING_STOP : MARKET),
        side(j["side"].get<std::string>() == "buy" ? BUY : SELL),
        position_intent(j["position_intent"].get<std::string>() == "buy_to_open" ? BUY_TO_OPEN : 
                        j["position_intent"].get<std::string>() == "buy_to_close" ? BUY_TO_CLOSE : 
                        j["position_intent"].get<std::string>() == "sell_to_open" ? SELL_TO_OPEN : 
                        j["position_intent"].get<std::string>() == "sell_to_close" ? SELL_TO_CLOSE : BUY_TO_OPEN),
        time_in_force(j["time_in_force"].get<std::string>() == "day" ? DAY :
                    j["time_in_force"].get<std::string>() == "gtc" ? GTC : 
                    j["time_in_force"].get<std::string>() == "opg" ? OPG : 
                    j["time_in_force"].get<std::string>() == "cls" ? CLS : 
                    j["time_in_force"].get<std::string>() == "ioc" ? IOC : FOK),
        limit_price(j["limit_price"].get<double>()),
        stop_price(j["stop_price"].get<double>()),
        status(j["status"].get<std::string>()),
        extended_hours(j["extended_hours"].get<bool>()),
        //legs(),
        trail_percent(j["trail_percent"].get<double>()),
        trail_price(j["trail_price"].get<double>()),
        hwm(j["hwm"].get<double>())
    {
        // Parse legs
        if (j.contains("legs")) {
            for (const auto& leg : j["legs"]) {
                legs.push_back(OrderResponse(leg));
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
        j["asset_class"] = asset_class == US_EQUITY ? "us_equity" : 
                           asset_class == US_OPTION ? "us_option" : "crypto";
        j["notional"] = notional;
        j["qty"] = qty;
        j["filled_qty"] = filled_qty;
        j["filled_avg_price"] = filled_avg_price;
        j["order_class"] = order_class == SIMPLE ? "simple" : 
                           order_class == OCO ? "oco" : 
                           order_class == BRACKET ? "bracket" : 
                           order_class == MLEG ? "mleg" : "oto";
        j["type"] = type == MARKET ? "market" :
                    type == LIMIT ? "limit" : 
                    type == STOP ? "stop" : 
                    type == STOP_LIMIT ? "stop_limit" : 
                    type == TRAILING_STOP ? "trailing_stop" : "unknown";
        j["side"] = side == BUY ? "buy" : "sell";
        j["position_intent"] = position_intent == BUY_TO_OPEN ? "buy_to_open" : 
                               position_intent == BUY_TO_CLOSE ? "buy_to_close" : 
                               position_intent == SELL_TO_OPEN ? "sell_to_open" : 
                               position_intent == SELL_TO_CLOSE ? "sell_to_close" : "unknown";
        j["time_in_force"] = time_in_force == DAY ? "day" :
                             time_in_force == GTC ? "gtc" : 
                             time_in_force == OPG ? "opg" :
                             time_in_force == CLS ? "cls" :
                             time_in_force == IOC ? "ioc" : "fok";
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