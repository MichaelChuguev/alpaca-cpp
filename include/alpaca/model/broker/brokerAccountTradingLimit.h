#pragma once

#include <string>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

struct BrokerUSDAccountTradingLimit {
    Decimal daily_net_limit;
    Decimal available;
    Decimal used;
    Decimal held;

    BrokerUSDAccountTradingLimit() = default;
    explicit BrokerUSDAccountTradingLimit(const json& j)
        : daily_net_limit(parse_decimal(j, "daily_net_limit")),
          available(parse_decimal(j, "available")),
          used(parse_decimal(j, "used")),
          held(parse_decimal(j, "held")) {}

    json to_json() const {
        json j;
        j["daily_net_limit"] = daily_net_limit.to_string();
        j["available"] = available.to_string();
        j["used"] = used.to_string();
        j["held"] = held.to_string();
        return j;
    }

    std::string to_string() const {
        return to_json().dump(2);
    }
};

struct BrokerAccountTradingLimit {
    Decimal daily_net_limit;
    Decimal available;
    Decimal used;
    Decimal held;
    Decimal swap_rate;
    BrokerUSDAccountTradingLimit usd;

    BrokerAccountTradingLimit() = default;
    explicit BrokerAccountTradingLimit(const json& j)
        : daily_net_limit(parse_decimal(j, "daily_net_limit")),
          available(parse_decimal(j, "available")),
          used(parse_decimal(j, "used")),
          held(parse_decimal(j, "held")),
          swap_rate(parse_decimal(j, "swap_rate")) {
        if (j.contains("usd") && j["usd"].is_object()) {
            usd = BrokerUSDAccountTradingLimit(j["usd"]);
        }
    }

    json to_json() const {
        json j;
        j["daily_net_limit"] = daily_net_limit.to_string();
        j["available"] = available.to_string();
        j["used"] = used.to_string();
        j["held"] = held.to_string();
        j["swap_rate"] = swap_rate.to_string();
        j["usd"] = usd.to_json();
        return j;
    }

    std::string to_string() const {
        return to_json().dump(2);
    }
};

} // namespace alpaca
