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

/// Parse a string field, returning "" if missing, null, or not a string.
inline std::string parse_string(const json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_string()) {
        return j[key].get<std::string>();
    }
    return "";
}

/// Parse a numeric value that the API may send as either a JSON string or number.
/// Returns Decimal{} (zero) if the field is missing or null.
inline Decimal parse_decimal(const json& j, const std::string& key) {
    if (!j.contains(key) || j[key].is_null()) return Decimal{};
    if (j[key].is_string()) return Decimal(j[key].get<std::string>());
    return Decimal(j[key].get<double>());
}

/// Parse an integer field, returning 0 if missing or null.
/// Handles API fields sent as either JSON strings or numbers.
inline int parse_int(const json& j, const std::string& key) {
    if (!j.contains(key) || j[key].is_null()) return 0;
    if (j[key].is_string()) return std::stoi(j[key].get<std::string>());
    return j[key].get<int>();
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

    QueryBuilder& add(const std::string& key, const Decimal& value) {
        if (value.is_positive()) {
            params_.emplace_back(key + "=" + value.to_string());
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
    return s == OrderSide::BUY ? "buy" : "sell";
}

inline OrderSide string_to_order_side(const std::string& s) {
    return s == "buy" ? OrderSide::BUY : OrderSide::SELL;
}

inline std::string order_type_to_string(OrderType t) {
    switch (t) {
        case OrderType::MARKET:        return "market";
        case OrderType::LIMIT:         return "limit";
        case OrderType::STOP:          return "stop";
        case OrderType::STOP_LIMIT:    return "stop_limit";
        case OrderType::TRAILING_STOP: return "trailing_stop";
    }
    return "market";
}

inline OrderType string_to_order_type(const std::string& s) {
    if (s == "market")        return OrderType::MARKET;
    if (s == "limit")         return OrderType::LIMIT;
    if (s == "stop")          return OrderType::STOP;
    if (s == "stop_limit")    return OrderType::STOP_LIMIT;
    if (s == "trailing_stop") return OrderType::TRAILING_STOP;
    return OrderType::MARKET;
}

inline std::string time_in_force_to_string(OrderTimeInForce t) {
    switch (t) {
        case OrderTimeInForce::DAY: return "day";
        case OrderTimeInForce::GTC: return "gtc";
        case OrderTimeInForce::OPG: return "opg";
        case OrderTimeInForce::CLS: return "cls";
        case OrderTimeInForce::IOC: return "ioc";
        case OrderTimeInForce::FOK: return "fok";
    }
    return "day";
}

inline OrderTimeInForce string_to_time_in_force(const std::string& s) {
    if (s == "day") return OrderTimeInForce::DAY;
    if (s == "gtc") return OrderTimeInForce::GTC;
    if (s == "opg") return OrderTimeInForce::OPG;
    if (s == "cls") return OrderTimeInForce::CLS;
    if (s == "ioc") return OrderTimeInForce::IOC;
    if (s == "fok") return OrderTimeInForce::FOK;
    return OrderTimeInForce::DAY;
}

inline std::string order_class_to_string(OrderClass c) {
    switch (c) {
        case OrderClass::SIMPLE:  return "simple";
        case OrderClass::BRACKET: return "bracket";
        case OrderClass::OCO:     return "oco";
        case OrderClass::OTO:     return "oto";
        case OrderClass::MLEG:    return "mleg";
    }
    return "simple";
}

inline OrderClass string_to_order_class(const std::string& s) {
    if (s == "simple")  return OrderClass::SIMPLE;
    if (s == "bracket") return OrderClass::BRACKET;
    if (s == "oco")     return OrderClass::OCO;
    if (s == "oto")     return OrderClass::OTO;
    if (s == "mleg")    return OrderClass::MLEG;
    return OrderClass::SIMPLE;
}

inline std::string position_intent_to_string(PositionIntent p) {
    switch (p) {
        case PositionIntent::BUY_TO_OPEN:  return "buy_to_open";
        case PositionIntent::BUY_TO_CLOSE: return "buy_to_close";
        case PositionIntent::SELL_TO_OPEN:  return "sell_to_open";
        case PositionIntent::SELL_TO_CLOSE: return "sell_to_close";
    }
    return "buy_to_open";
}

inline PositionIntent string_to_position_intent(const std::string& s) {
    if (s == "buy_to_open")  return PositionIntent::BUY_TO_OPEN;
    if (s == "buy_to_close") return PositionIntent::BUY_TO_CLOSE;
    if (s == "sell_to_open")  return PositionIntent::SELL_TO_OPEN;
    if (s == "sell_to_close") return PositionIntent::SELL_TO_CLOSE;
    return PositionIntent::BUY_TO_OPEN;
}

inline std::string asset_class_to_string(AssetClass c) {
    switch (c) {
        case AssetClass::US_EQUITY: return "us_equity";
        case AssetClass::US_OPTION: return "us_option";
        case AssetClass::CRYPTO:    return "crypto";
    }
    return "us_equity";
}

inline AssetClass string_to_asset_class(const std::string& s) {
    if (s == "us_equity") return AssetClass::US_EQUITY;
    if (s == "us_option") return AssetClass::US_OPTION;
    if (s == "crypto")    return AssetClass::CRYPTO;
    return AssetClass::US_EQUITY;
}

inline std::string exchange_to_string(Exchange e) {
    switch (e) {
        case Exchange::AMEX:    return "AMEX";
        case Exchange::ARCA:    return "ARCA";
        case Exchange::BATS:    return "BATS";
        case Exchange::NYSE:    return "NYSE";
        case Exchange::NASDAQ:  return "NASDAQ";
        case Exchange::NYSEARCA:return "NYSEARCA";
        case Exchange::OTC:     return "OTC";
    }
    return "NYSE";
}

inline Exchange string_to_exchange(const std::string& s) {
    if (s == "AMEX")    return Exchange::AMEX;
    if (s == "ARCA")    return Exchange::ARCA;
    if (s == "BATS")    return Exchange::BATS;
    if (s == "NYSE")    return Exchange::NYSE;
    if (s == "NASDAQ")  return Exchange::NASDAQ;
    if (s == "NYSEARCA")return Exchange::NYSEARCA;
    if (s == "OTC")     return Exchange::OTC;
    return Exchange::NYSE;
}

inline std::string sort_to_string(Sort s) {
    return s == Sort::ASC ? "asc" : "desc";
}

// ---------------------------------------------------------------------------
// Market Data enum ↔ string helpers
// ---------------------------------------------------------------------------

inline std::string data_timeframe_to_string(DataTimeframe tf) {
    switch (tf) {
        case DataTimeframe::ONE_MIN:      return "1Min";
        case DataTimeframe::FIVE_MIN:     return "5Min";
        case DataTimeframe::FIFTEEN_MIN:  return "15Min";
        case DataTimeframe::THIRTY_MIN:   return "30Min";
        case DataTimeframe::ONE_HOUR:     return "1Hour";
        case DataTimeframe::ONE_DAY:      return "1Day";
        case DataTimeframe::ONE_WEEK:     return "1Week";
        case DataTimeframe::ONE_MONTH:    return "1Month";
    }
    return "1Day";
}

inline std::string data_feed_to_string(DataFeed f) {
    switch (f) {
        case DataFeed::IEX: return "iex";
        case DataFeed::SIP: return "sip";
        case DataFeed::OTC: return "otc";
        case DataFeed::DEFAULT: return "iex"; // fallback; should be resolved before reaching here
    }
    return "iex";
}

inline std::string option_feed_to_string(OptionFeed f) {
    switch (f) {
        case OptionFeed::INDICATIVE: return "indicative";
        case OptionFeed::OPRA: return "opra";
        case OptionFeed::DEFAULT: return "";
    }
    return "";
}

inline std::string crypto_loc_to_string(CryptoLoc loc) {
    switch (loc) {
        case CryptoLoc::US: return "us";
    }
    return "us";
}

inline std::string screener_market_type_to_string(ScreenerMarketType mt) {
    switch (mt) {
        case ScreenerMarketType::STOCKS: return "stocks";
        case ScreenerMarketType::CRYPTO: return "crypto";
    }
    return "stocks";
}

inline std::string tick_type_to_string(TickType tt) {
    switch (tt) {
        case TickType::TRADES: return "trades";
        case TickType::QUOTES: return "quotes";
    }
    return "trades";
}

inline std::string corporate_action_type_to_string(CorporateActionType t) {
    switch (t) {
        case CorporateActionType::REVERSE_SPLIT:          return "reverse_split";
        case CorporateActionType::FORWARD_SPLIT:          return "forward_split";
        case CorporateActionType::UNIT_SPLIT:             return "unit_split";
        case CorporateActionType::CASH_DIVIDEND:          return "cash_dividend";
        case CorporateActionType::STOCK_DIVIDEND:         return "stock_dividend";
        case CorporateActionType::SPIN_OFF:               return "spin_off";
        case CorporateActionType::CASH_MERGER:            return "cash_merger";
        case CorporateActionType::STOCK_MERGER:           return "stock_merger";
        case CorporateActionType::STOCK_AND_CASH_MERGER:  return "stock_and_cash_merger";
        case CorporateActionType::REDEMPTION:             return "redemption";
        case CorporateActionType::NAME_CHANGE:            return "name_change";
        case CorporateActionType::WORTHLESS_REMOVAL:      return "worthless_removal";
        case CorporateActionType::RIGHTS_DISTRIBUTION:    return "rights_distribution";
        case CorporateActionType::CONTRACT_ADJUSTMENT:    return "contract_adjustment";
    }
    return "cash_dividend";
}

} // namespace alpaca
