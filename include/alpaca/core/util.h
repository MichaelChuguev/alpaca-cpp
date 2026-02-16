#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <optional>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// JSON parsing helpers — eliminate repeated contains/null-check boilerplate
// ---------------------------------------------------------------------------

/// Parse a string field, returning "" if missing or null.
inline std::string parse_string(const json& j, const std::string& key) {
    if (j.contains(key) && !j[key].is_null()) {
        return j[key].get<std::string>();
    }
    return "";
}

/// Parse a numeric value that the API may send as either a JSON string or number.
/// Returns 0.0 if the field is missing or null.
inline double parse_decimal(const json& j, const std::string& key) {
    if (!j.contains(key) || j[key].is_null()) return 0.0;
    if (j[key].is_string()) return std::stod(j[key].get<std::string>());
    return j[key].get<double>();
}

/// Parse a bool field, returning false if missing or null.
inline bool parse_bool(const json& j, const std::string& key) {
    if (j.contains(key) && !j[key].is_null()) {
        return j[key].get<bool>();
    }
    return false;
}

/// Parse a DateTime field, returning a default DateTime() if missing or null.
inline DateTime parse_datetime(const json& j, const std::string& key) {
    if (j.contains(key) && !j[key].is_null()) {
        return DateTime::parse(j[key]);
    }
    return DateTime();
}

// ---------------------------------------------------------------------------
// Query-string builder — eliminates repeated append-and-trim boilerplate
// ---------------------------------------------------------------------------

class QueryBuilder {
public:
    explicit QueryBuilder(const std::string& base_path)
        : path_(base_path) {}

    QueryBuilder& add(const std::string& key, const std::string& value) {
        if (!value.empty()) {
            params_.emplace_back(key + "=" + value);
        }
        return *this;
    }

    QueryBuilder& add(const std::string& key, int value) {
        if (value > 0) {
            params_.emplace_back(key + "=" + std::to_string(value));
        }
        return *this;
    }

    QueryBuilder& add(const std::string& key, double value) {
        if (value > 0) {
            params_.emplace_back(key + "=" + std::to_string(value));
        }
        return *this;
    }

    QueryBuilder& add(const std::string& key, bool value) {
        params_.emplace_back(key + "=" + std::string(value ? "true" : "false"));
        return *this;
    }

    QueryBuilder& add_list(const std::string& key, const std::vector<std::string>& values) {
        if (!values.empty()) {
            std::string joined;
            for (size_t i = 0; i < values.size(); ++i) {
                joined += values[i];
                if (i + 1 < values.size()) joined += ",";
            }
            params_.emplace_back(key + "=" + joined);
        }
        return *this;
    }

    QueryBuilder& add_date(const std::string& key, const DateTime& dt) {
        if (dt.to_epoch_seconds() > 0 && dt.is_date_only()) {
            params_.emplace_back(key + "=" + dt.to_iso_string());
        }
        return *this;
    }

    std::string build() const {
        if (params_.empty()) return path_;
        std::string result = path_ + "?";
        for (size_t i = 0; i < params_.size(); ++i) {
            result += params_[i];
            if (i + 1 < params_.size()) result += "&";
        }
        return result;
    }

private:
    std::string path_;
    std::vector<std::string> params_;
};

// ---------------------------------------------------------------------------
// URL encoding
// ---------------------------------------------------------------------------

inline std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;
    for (char c : value) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase
                    << '%' << std::setw(2) << int(static_cast<unsigned char>(c))
                    << std::nouppercase;
        }
    }
    return escaped.str();
}

// ---------------------------------------------------------------------------
// Enum ↔ string conversion helpers
// ---------------------------------------------------------------------------

inline std::string order_side_to_string(OrderSide s) {
    return s == BUY ? "buy" : "sell";
}

inline OrderSide string_to_order_side(const std::string& s) {
    return s == "buy" ? BUY : SELL;
}

inline std::string order_type_to_string(OrderType t) {
    switch (t) {
        case MARKET:        return "market";
        case LIMIT:         return "limit";
        case STOP:          return "stop";
        case STOP_LIMIT:    return "stop_limit";
        case TRAILING_STOP: return "trailing_stop";
        default:            return "market";
    }
}

inline OrderType string_to_order_type(const std::string& s) {
    if (s == "market")        return MARKET;
    if (s == "limit")         return LIMIT;
    if (s == "stop")          return STOP;
    if (s == "stop_limit")    return STOP_LIMIT;
    if (s == "trailing_stop") return TRAILING_STOP;
    return MARKET;
}

inline std::string time_in_force_to_string(OrderTimeInForce t) {
    switch (t) {
        case DAY: return "day";
        case GTC: return "gtc";
        case OPG: return "opg";
        case CLS: return "cls";
        case IOC: return "ioc";
        case FOK: return "fok";
        default:  return "day";
    }
}

inline OrderTimeInForce string_to_time_in_force(const std::string& s) {
    if (s == "day") return DAY;
    if (s == "gtc") return GTC;
    if (s == "opg") return OPG;
    if (s == "cls") return CLS;
    if (s == "ioc") return IOC;
    if (s == "fok") return FOK;
    return DAY;
}

inline std::string order_class_to_string(OrderClass c) {
    switch (c) {
        case SIMPLE:  return "simple";
        case BRACKET: return "bracket";
        case OCO:     return "oco";
        case OTO:     return "oto";
        case MLEG:    return "mleg";
        default:      return "simple";
    }
}

inline OrderClass string_to_order_class(const std::string& s) {
    if (s == "simple")  return SIMPLE;
    if (s == "bracket") return BRACKET;
    if (s == "oco")     return OCO;
    if (s == "oto")     return OTO;
    if (s == "mleg")    return MLEG;
    return SIMPLE;
}

inline std::string position_intent_to_string(PositionIntent p) {
    switch (p) {
        case BUY_TO_OPEN:  return "buy_to_open";
        case BUY_TO_CLOSE: return "buy_to_close";
        case SELL_TO_OPEN:  return "sell_to_open";
        case SELL_TO_CLOSE: return "sell_to_close";
        default:            return "buy_to_open";
    }
}

inline PositionIntent string_to_position_intent(const std::string& s) {
    if (s == "buy_to_open")  return BUY_TO_OPEN;
    if (s == "buy_to_close") return BUY_TO_CLOSE;
    if (s == "sell_to_open")  return SELL_TO_OPEN;
    if (s == "sell_to_close") return SELL_TO_CLOSE;
    return BUY_TO_OPEN;
}

inline std::string asset_class_to_string(AssetClass c) {
    switch (c) {
        case US_EQUITY: return "us_equity";
        case US_OPTION: return "us_option";
        case CRYPTO:    return "crypto";
        default:        return "us_equity";
    }
}

inline AssetClass string_to_asset_class(const std::string& s) {
    if (s == "us_equity") return US_EQUITY;
    if (s == "us_option") return US_OPTION;
    if (s == "crypto")    return CRYPTO;
    return US_EQUITY;
}

inline std::string exchange_to_string(Exchange e) {
    switch (e) {
        case AMEX:    return "AMEX";
        case ARCA:    return "ARCA";
        case BATS:    return "BATS";
        case NYSE:    return "NYSE";
        case NASDAQ:  return "NASDAQ";
        case NYSEARCA:return "NYSEARCA";
        case OTC:     return "OTC";
        default:      return "NYSE";
    }
}

inline Exchange string_to_exchange(const std::string& s) {
    if (s == "AMEX")    return AMEX;
    if (s == "ARCA")    return ARCA;
    if (s == "BATS")    return BATS;
    if (s == "NYSE")    return NYSE;
    if (s == "NASDAQ")  return NASDAQ;
    if (s == "NYSEARCA")return NYSEARCA;
    if (s == "OTC")     return OTC;
    return NYSE;
}

inline std::string sort_to_string(Sort s) {
    return s == ASC ? "asc" : "desc";
}

} // namespace alpaca
