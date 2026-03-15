#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"
#include "alpaca/core/util.h"
#include "alpaca/model/trader/order.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Trade-update event types (trade_updates stream)
// ---------------------------------------------------------------------------

enum class TradeUpdateEvent {
    NEW,
    FILL,
    PARTIAL_FILL,
    CANCELED,
    EXPIRED,
    DONE_FOR_DAY,
    REPLACED,
    ACCEPTED,
    REJECTED,
    PENDING_NEW,
    STOPPED,
    PENDING_CANCEL,
    PENDING_REPLACE,
    CALCULATED,
    SUSPENDED,
    ORDER_REPLACE_REJECTED,
    ORDER_CANCEL_REJECTED,
    UNKNOWN
};

inline std::string trade_update_event_to_string(TradeUpdateEvent e) {
    switch (e) {
        case TradeUpdateEvent::NEW:                    return "new";
        case TradeUpdateEvent::FILL:                   return "fill";
        case TradeUpdateEvent::PARTIAL_FILL:           return "partial_fill";
        case TradeUpdateEvent::CANCELED:               return "canceled";
        case TradeUpdateEvent::EXPIRED:                return "expired";
        case TradeUpdateEvent::DONE_FOR_DAY:           return "done_for_day";
        case TradeUpdateEvent::REPLACED:               return "replaced";
        case TradeUpdateEvent::ACCEPTED:               return "accepted";
        case TradeUpdateEvent::REJECTED:               return "rejected";
        case TradeUpdateEvent::PENDING_NEW:            return "pending_new";
        case TradeUpdateEvent::STOPPED:                return "stopped";
        case TradeUpdateEvent::PENDING_CANCEL:         return "pending_cancel";
        case TradeUpdateEvent::PENDING_REPLACE:        return "pending_replace";
        case TradeUpdateEvent::CALCULATED:             return "calculated";
        case TradeUpdateEvent::SUSPENDED:              return "suspended";
        case TradeUpdateEvent::ORDER_REPLACE_REJECTED: return "order_replace_rejected";
        case TradeUpdateEvent::ORDER_CANCEL_REJECTED:  return "order_cancel_rejected";
        default:                                       return "unknown";
    }
}

inline TradeUpdateEvent string_to_trade_update_event(const std::string& s) {
    if (s == "new")                    return TradeUpdateEvent::NEW;
    if (s == "fill")                   return TradeUpdateEvent::FILL;
    if (s == "partial_fill")           return TradeUpdateEvent::PARTIAL_FILL;
    if (s == "canceled")               return TradeUpdateEvent::CANCELED;
    if (s == "expired")                return TradeUpdateEvent::EXPIRED;
    if (s == "done_for_day")           return TradeUpdateEvent::DONE_FOR_DAY;
    if (s == "replaced")               return TradeUpdateEvent::REPLACED;
    if (s == "accepted")               return TradeUpdateEvent::ACCEPTED;
    if (s == "rejected")               return TradeUpdateEvent::REJECTED;
    if (s == "pending_new")            return TradeUpdateEvent::PENDING_NEW;
    if (s == "stopped")                return TradeUpdateEvent::STOPPED;
    if (s == "pending_cancel")         return TradeUpdateEvent::PENDING_CANCEL;
    if (s == "pending_replace")        return TradeUpdateEvent::PENDING_REPLACE;
    if (s == "calculated")             return TradeUpdateEvent::CALCULATED;
    if (s == "suspended")              return TradeUpdateEvent::SUSPENDED;
    if (s == "order_replace_rejected") return TradeUpdateEvent::ORDER_REPLACE_REJECTED;
    if (s == "order_cancel_rejected")  return TradeUpdateEvent::ORDER_CANCEL_REJECTED;
    return TradeUpdateEvent::UNKNOWN;
}

// ---------------------------------------------------------------------------
// TradeUpdate — a single event from the trade_updates stream
// ---------------------------------------------------------------------------

struct TradeUpdate {
    TradeUpdateEvent event;
    std::string      execution_id;
    std::optional<OrderResponse> order;  // reuses REST order model
    Decimal          price{};
    Decimal          qty{};
    Decimal          position_qty{};
    DateTime         timestamp;

    /// Parse from the "data" object inside a trade_updates message.
    static TradeUpdate from_json(const nlohmann::json& j) {
        TradeUpdate tu;
        tu.event        = string_to_trade_update_event(parse_string(j, "event"));
        tu.execution_id = parse_string(j, "execution_id");
        tu.price        = parse_decimal(j, "price");
        tu.qty          = parse_decimal(j, "qty");
        tu.position_qty = parse_decimal(j, "position_qty");
        tu.timestamp    = parse_datetime(j, "timestamp");

        if (j.contains("order") && !j["order"].is_null()) {
            tu.order.emplace(j["order"]);
        }
        return tu;
    }
};

} // namespace alpaca
