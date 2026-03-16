#pragma once

#include <string>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

enum class PDTRemovalReason {
    UNSET,
    BELOW_THRESHOLD,
    ABOVE_THRESHOLD
};

inline PDTRemovalReason pdt_removal_reason_from_string(const std::string& s) {
    if (s == "BELOW_THRESHOLD") return PDTRemovalReason::BELOW_THRESHOLD;
    if (s == "ABOVE_THRESHOLD") return PDTRemovalReason::ABOVE_THRESHOLD;
    return PDTRemovalReason::UNSET;
}

inline std::string pdt_removal_reason_to_string(PDTRemovalReason r) {
    switch (r) {
        case PDTRemovalReason::BELOW_THRESHOLD: return "BELOW_THRESHOLD";
        case PDTRemovalReason::ABOVE_THRESHOLD: return "ABOVE_THRESHOLD";
        case PDTRemovalReason::UNSET:           return "";
    }
    return "";
}

// ---------------------------------------------------------------------------
// PDTStatus — model for PDT status / one-time removal responses
// ---------------------------------------------------------------------------

struct PDTStatus {
    bool is_pattern_day_trader = false;
    int daytrade_count = 0;
    int last_daytrade_count = 0;
    Decimal daytrading_buying_power;
    Decimal last_daytrading_buying_power;
    Decimal equity;
    bool one_time_removal_used = false;
    PDTRemovalReason removal_reason = PDTRemovalReason::UNSET;
    DateTime flagged_at;
    DateTime removal_at;

    PDTStatus() = default;
    PDTStatus(const json& j)
        : is_pattern_day_trader(parse_bool(j, "is_pattern_day_trader")),
          daytrade_count(parse_int(j, "daytrade_count")),
          last_daytrade_count(parse_int(j, "last_daytrade_count")),
          daytrading_buying_power(parse_decimal(j, "daytrading_buying_power")),
          last_daytrading_buying_power(parse_decimal(j, "last_daytrading_buying_power")),
          equity(parse_decimal(j, "equity")),
          one_time_removal_used(parse_bool(j, "one_time_removal_used")),
          removal_reason(pdt_removal_reason_from_string(parse_string(j, "removal_reason"))),
          flagged_at(parse_datetime(j, "flagged_at")),
          removal_at(parse_datetime(j, "removal_at"))
    {}

    std::string to_string() const {
        std::string s = "PDTStatus { is_pattern_day_trader=" + std::string(is_pattern_day_trader ? "true" : "false")
            + ", daytrade_count=" + std::to_string(daytrade_count)
            + ", last_daytrade_count=" + std::to_string(last_daytrade_count)
            + ", daytrading_buying_power=" + daytrading_buying_power.to_string()
            + ", last_daytrading_buying_power=" + last_daytrading_buying_power.to_string()
            + ", equity=" + equity.to_string()
            + ", one_time_removal_used=" + std::string(one_time_removal_used ? "true" : "false")
            + ", removal_reason=" + pdt_removal_reason_to_string(removal_reason)
            + " }";
        return s;
    }
};

} // namespace alpaca
