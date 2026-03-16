#pragma once

#include <string>
#include <sstream>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ── Enums ─────────────────────────────────────────────────────────────────────

enum class ActivityType {
    UNSET,
    FILL,
    ACATC,
    ACATS,
    ACATSC,
    ACATSS,
    CSD,
    CSW,
    DIV,
    DIVNRA,
    FEE,
    INT,
    JNLC,
    JNLS,
    MERGER,
    MISC,
    OPEXP,
    PEI,
    PTA,
    PTC,
    PTB,
    REORG,
    SPIN,
    SPO,
    SSO,
    SSP,
    STDV,
    SWAP
};

inline ActivityType activity_type_from_string(const std::string& s) {
    if (s == "FILL")   return ActivityType::FILL;
    if (s == "ACATC")  return ActivityType::ACATC;
    if (s == "ACATS")  return ActivityType::ACATS;
    if (s == "ACATSC") return ActivityType::ACATSC;
    if (s == "ACATSS") return ActivityType::ACATSS;
    if (s == "CSD")    return ActivityType::CSD;
    if (s == "CSW")    return ActivityType::CSW;
    if (s == "DIV")    return ActivityType::DIV;
    if (s == "DIVNRA") return ActivityType::DIVNRA;
    if (s == "FEE")    return ActivityType::FEE;
    if (s == "INT")    return ActivityType::INT;
    if (s == "JNLC")   return ActivityType::JNLC;
    if (s == "JNLS")   return ActivityType::JNLS;
    if (s == "MERGER") return ActivityType::MERGER;
    if (s == "MISC")   return ActivityType::MISC;
    if (s == "OPEXP")  return ActivityType::OPEXP;
    if (s == "PEI")    return ActivityType::PEI;
    if (s == "PTA")    return ActivityType::PTA;
    if (s == "PTC")    return ActivityType::PTC;
    if (s == "PTB")    return ActivityType::PTB;
    if (s == "REORG")  return ActivityType::REORG;
    if (s == "SPIN")   return ActivityType::SPIN;
    if (s == "SPO")    return ActivityType::SPO;
    if (s == "SSO")    return ActivityType::SSO;
    if (s == "SSP")    return ActivityType::SSP;
    if (s == "STDV")   return ActivityType::STDV;
    if (s == "SWAP")   return ActivityType::SWAP;
    return ActivityType::UNSET;
}

inline std::string activity_type_to_string(ActivityType t) {
    switch (t) {
        case ActivityType::FILL:   return "FILL";
        case ActivityType::ACATC:  return "ACATC";
        case ActivityType::ACATS:  return "ACATS";
        case ActivityType::ACATSC: return "ACATSC";
        case ActivityType::ACATSS: return "ACATSS";
        case ActivityType::CSD:    return "CSD";
        case ActivityType::CSW:    return "CSW";
        case ActivityType::DIV:    return "DIV";
        case ActivityType::DIVNRA: return "DIVNRA";
        case ActivityType::FEE:    return "FEE";
        case ActivityType::INT:    return "INT";
        case ActivityType::JNLC:   return "JNLC";
        case ActivityType::JNLS:   return "JNLS";
        case ActivityType::MERGER: return "MERGER";
        case ActivityType::MISC:   return "MISC";
        case ActivityType::OPEXP:  return "OPEXP";
        case ActivityType::PEI:    return "PEI";
        case ActivityType::PTA:    return "PTA";
        case ActivityType::PTC:    return "PTC";
        case ActivityType::PTB:    return "PTB";
        case ActivityType::REORG:  return "REORG";
        case ActivityType::SPIN:   return "SPIN";
        case ActivityType::SPO:    return "SPO";
        case ActivityType::SSO:    return "SSO";
        case ActivityType::SSP:    return "SSP";
        case ActivityType::STDV:   return "STDV";
        case ActivityType::SWAP:   return "SWAP";
        case ActivityType::UNSET:  return "";
    }
    return "";
}

// Category filter — mutually exclusive with activity_types on GET /v1/accounts/activities
enum class ActivityCategory {
    UNSET,
    TRADE_ACTIVITY,
    NON_TRADE_ACTIVITY
};

inline ActivityCategory activity_category_from_string(const std::string& s) {
    if (s == "trade_activity")     return ActivityCategory::TRADE_ACTIVITY;
    if (s == "non_trade_activity") return ActivityCategory::NON_TRADE_ACTIVITY;
    return ActivityCategory::UNSET;
}

inline std::string activity_category_to_string(ActivityCategory c) {
    switch (c) {
        case ActivityCategory::TRADE_ACTIVITY:     return "trade_activity";
        case ActivityCategory::NON_TRADE_ACTIVITY: return "non_trade_activity";
        case ActivityCategory::UNSET:              return "";
    }
    return "";
}

// Settlement status for non-trade activities
enum class NonTradeActivityStatus {
    UNSET,
    EXECUTED,
    CORRECT,
    CANCELED
};

inline NonTradeActivityStatus nta_status_from_string(const std::string& s) {
    if (s == "executed") return NonTradeActivityStatus::EXECUTED;
    if (s == "correct")  return NonTradeActivityStatus::CORRECT;
    if (s == "canceled") return NonTradeActivityStatus::CANCELED;
    return NonTradeActivityStatus::UNSET;
}

inline std::string nta_status_to_string(NonTradeActivityStatus s) {
    switch (s) {
        case NonTradeActivityStatus::EXECUTED: return "executed";
        case NonTradeActivityStatus::CORRECT:  return "correct";
        case NonTradeActivityStatus::CANCELED: return "canceled";
        case NonTradeActivityStatus::UNSET:    return "";
    }
    return "";
}

// Fill completeness for trade activities
enum class TradeActivityType {
    UNSET,
    FILL,
    PARTIAL_FILL
};

inline TradeActivityType trade_activity_type_from_string(const std::string& s) {
    if (s == "fill")         return TradeActivityType::FILL;
    if (s == "partial_fill") return TradeActivityType::PARTIAL_FILL;
    return TradeActivityType::UNSET;
}

inline std::string trade_activity_type_to_string(TradeActivityType t) {
    switch (t) {
        case TradeActivityType::FILL:         return "fill";
        case TradeActivityType::PARTIAL_FILL: return "partial_fill";
        case TradeActivityType::UNSET:        return "";
    }
    return "";
}

// ── BrokerAccountActivity ────────────────────────────────────────────────────────────

struct BrokerAccountActivity {

    // Base fields
    std::string id;
    std::string account_id;
    ActivityType activity_type = ActivityType::UNSET;
    std::string account_activity_type; // "trade_activity" | "non_trade_activity" | "other_activity"

    // Shared
    std::string symbol;
    Decimal qty;

    // Trade activity fields (activity_type == FILL)
    DateTime transaction_time;
    TradeActivityType type = TradeActivityType::UNSET;
    Decimal price;
    OrderSide side = OrderSide::BUY;
    Decimal leaves_qty;
    std::string order_id;
    Decimal cum_qty;
    std::string order_status;

    // Non-trade activity fields
    DateTime date;
    Decimal net_amount;
    std::string description;
    NonTradeActivityStatus status = NonTradeActivityStatus::UNSET;
    std::string cusip;
    Decimal per_share_amount;
    std::string currency;
    std::string activity_sub_type;
    std::string group_id;
    DateTime created_at;

    BrokerAccountActivity() = default;

    BrokerAccountActivity(const json& j) {
        id            = parse_string(j, "id");
        account_id    = parse_string(j, "account_id");
        activity_type = activity_type_from_string(parse_string(j, "activity_type"));
        symbol        = parse_string(j, "symbol");
        qty           = parse_decimal(j, "qty");

        if (j.contains("order_id") && j.contains("order_status") && j.contains("type") &&
            j.contains("cum_qty") && j.contains("leaves_qty") && j.contains("price") &&
            j.contains("side") && j.contains("transaction_time")) {

            account_activity_type = "trade_activity";
            transaction_time = parse_datetime(j, "transaction_time");
            type             = trade_activity_type_from_string(parse_string(j, "type"));
            price            = parse_decimal(j, "price");
            side             = string_to_order_side(parse_string(j, "side"));
            leaves_qty       = parse_decimal(j, "leaves_qty");
            order_id         = parse_string(j, "order_id");
            cum_qty          = parse_decimal(j, "cum_qty");
            order_status     = parse_string(j, "order_status");

        } else if (j.contains("date") && j.contains("net_amount") &&
                   j.contains("description") && j.contains("status")) {

            account_activity_type = "non_trade_activity";
            date              = parse_datetime(j, "date");
            net_amount        = parse_decimal(j, "net_amount");
            description       = parse_string(j, "description");
            status            = nta_status_from_string(parse_string(j, "status"));
            cusip             = parse_string(j, "cusip");
            per_share_amount  = parse_decimal(j, "per_share_amount");
            currency          = parse_string(j, "currency");
            activity_sub_type = parse_string(j, "activity_sub_type");
            group_id          = parse_string(j, "group_id");
            created_at        = parse_datetime(j, "created_at");

        } else {
            account_activity_type = "other_activity";
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "BrokerAccountActivity:\n"
            << "  id:           " << id << "\n"
            << "  account_id:   " << account_id << "\n"
            << "  type:         " << account_activity_type << "\n"
            << "  activity:     " << activity_type_to_string(activity_type) << "\n";

        if (account_activity_type == "trade_activity") {
            oss << "  symbol:        " << symbol << "\n"
                << "  qty:           " << qty.to_double() << "\n"
                << "  cum_qty:       " << cum_qty.to_double() << "\n"
                << "  leaves_qty:    " << leaves_qty.to_double() << "\n"
                << "  price:         " << price.to_double() << "\n"
                << "  side:          " << order_side_to_string(side) << "\n"
                << "  order_id:      " << order_id << "\n"
                << "  fill_type:     " << trade_activity_type_to_string(type) << "\n"
                << "  order_status:  " << order_status << "\n";
        } else if (account_activity_type == "non_trade_activity") {
            oss << "  symbol:            " << symbol << "\n"
                << "  qty:               " << qty.to_double() << "\n"
                << "  date:              " << date.to_string() << "\n"
                << "  net_amount:        " << net_amount.to_double() << "\n"
                << "  description:       " << description << "\n"
                << "  status:            " << nta_status_to_string(status) << "\n"
                << "  cusip:             " << cusip << "\n"
                << "  per_share_amount:  " << per_share_amount.to_double() << "\n"
                << "  currency:          " << currency << "\n"
                << "  activity_sub_type: " << activity_sub_type << "\n"
                << "  group_id:          " << group_id << "\n"
                << "  created_at:        " << created_at.to_string() << "\n";
        }

        return oss.str();
    }
};

} // namespace alpaca
