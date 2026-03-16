#pragma once

#include <chrono>
#include <cmath>
#include <compare>
#include <cstdint>
#include <date/date.h>
#include <ostream>
#include <string>
#include <optional>

#include "nlohmann/json.hpp"

namespace alpaca {

// ---------------------------------------------------------------------------
// Decimal — fixed-point type for financial values (8 decimal places)
// ---------------------------------------------------------------------------

class Decimal {
public:
    static constexpr int SCALE = 8;
    static constexpr int64_t SCALE_FACTOR = 100'000'000LL;

    constexpr Decimal() noexcept = default;
    explicit Decimal(const std::string& str);
    explicit Decimal(double val) noexcept
        : mantissa_(std::llround(val * static_cast<double>(SCALE_FACTOR))) {}

    [[nodiscard]] double to_double() const noexcept {
        return static_cast<double>(mantissa_) / static_cast<double>(SCALE_FACTOR);
    }
    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] constexpr bool is_zero() const noexcept { return mantissa_ == 0; }
    [[nodiscard]] constexpr bool is_positive() const noexcept { return mantissa_ > 0; }
    [[nodiscard]] constexpr bool is_negative() const noexcept { return mantissa_ < 0; }

    constexpr auto operator<=>(const Decimal&) const noexcept = default;
    constexpr bool operator==(const Decimal&) const noexcept = default;

    constexpr Decimal operator+(const Decimal& rhs) const noexcept {
        Decimal r; r.mantissa_ = mantissa_ + rhs.mantissa_; return r;
    }
    constexpr Decimal operator-(const Decimal& rhs) const noexcept {
        Decimal r; r.mantissa_ = mantissa_ - rhs.mantissa_; return r;
    }
    constexpr Decimal operator-() const noexcept {
        Decimal r; r.mantissa_ = -mantissa_; return r;
    }
    Decimal operator*(const Decimal& rhs) const noexcept;
    Decimal operator/(const Decimal& rhs) const noexcept;

    // Heterogeneous comparison with int (common pattern: if (price > 0))
    friend constexpr auto operator<=>(const Decimal& lhs, int rhs) noexcept {
        return lhs.mantissa_ <=> (static_cast<int64_t>(rhs) * SCALE_FACTOR);
    }
    friend constexpr bool operator==(const Decimal& lhs, int rhs) noexcept {
        return lhs.mantissa_ == static_cast<int64_t>(rhs) * SCALE_FACTOR;
    }

    friend std::ostream& operator<<(std::ostream& os, const Decimal& d) {
        return os << d.to_string();
    }

private:
    int64_t mantissa_ = 0;
};

// ---------------------------------------------------------------------------
// DateTime
// ---------------------------------------------------------------------------

/**
 * @brief Handles all Alpaca datetime formats with timezone support
 * 
 * Supports:
 * - 2024-01-03T01:02:03.123456789Z
 * - 2024-01-04T00:00:00Z
 * - 2024-01-03T09:30:00-04:00 
 * - 2024-01-04 (date-only)
 * 
 * - 0930
 * - 09:30
 * 
 */
class DateTime {
public:
    using sys_time = std::chrono::system_clock::time_point;
    
    // Construction
    DateTime() = default;
    explicit DateTime(const std::string& iso_str);
    explicit DateTime(sys_time tp, std::optional<int> offset_minutes = std::nullopt);
    explicit DateTime(int64_t unix_epoch_seconds);
    
    // Parsing
    static DateTime parse(const nlohmann::json& j);
    static DateTime now();
    
    // Conversion
    std::string to_iso_string(bool as_epoch = false) const;
    int64_t to_epoch_seconds() const noexcept;
    sys_time to_time_point() const noexcept;
    std::optional<int> utc_offset_minutes() const noexcept;
    
    // Comparisons (C++20 spaceship — generates <, >, <=, >=, !=)
    std::strong_ordering operator<=>(const DateTime&) const noexcept;
    bool operator==(const DateTime&) const noexcept;
    
    // Utility
    bool is_date_only() const noexcept;
    std::string to_string() const { return to_iso_string(); }

private:
    sys_time time_point_;
    std::optional<int> offset_minutes_; // UTC offset in minutes
    
    // Parsing helpers
    static void parse_iso_datetime(const std::string&, sys_time&, std::optional<int>&);
    static void parse_iso_date(const std::string&, sys_time&);
};

using Timestamp = DateTime;

enum class AuthAPIEndpoint { SANDBOX, PRODUCTION };
enum class TraderAPIEndpoint { PAPER, LIVE };
enum class MarketDataWebsocketSource { IEX, SIP };
enum class MarketDataEndpoint { SANDBOX, LIVE };
enum class BrokerAPIEndpoint { SANDBOX, PRODUCTION };

// Authentication mode for Broker API.
// LEGACY            – send APCA-API-KEY-ID / APCA-API-SECRET-KEY headers directly.
// CLIENT_CREDENTIALS – exchange client_id+client_secret for a short-lived OAuth2
//                      Bearer token at authx.alpaca.markets (recommended).
enum class BrokerAuthMode { LEGACY, CLIENT_CREDENTIALS };
enum class OptionsApprovalStatus { UNSET, PENDING, APPROVED, LOWER_LEVEL_APPROVED, REJECTED };
enum class Sort { ASC, DESC };
enum class AssetClass { US_EQUITY, US_OPTION, CRYPTO };
enum class Exchange { AMEX, ARCA, BATS, NYSE, NASDAQ, NYSEARCA, OTC };
enum class OrderSide { BUY, SELL };
enum class OrderType { MARKET, LIMIT, STOP, STOP_LIMIT, TRAILING_STOP };
enum class OrderTimeInForce { DAY, GTC, OPG, CLS, IOC, FOK };
enum class OrderClass { SIMPLE, BRACKET, OCO, OTO, MLEG };
enum class PositionIntent { BUY_TO_OPEN, BUY_TO_CLOSE, SELL_TO_OPEN, SELL_TO_CLOSE };

// Market Data specific enums
enum class DataTimeframe {
    ONE_MIN, FIVE_MIN, FIFTEEN_MIN, THIRTY_MIN,
    ONE_HOUR, ONE_DAY, ONE_WEEK, ONE_MONTH
};
enum class DataFeed { IEX, SIP, OTC, DEFAULT };
enum class CryptoLoc { US };
enum class ScreenerMarketType { STOCKS, CRYPTO };
enum class TickType { TRADES, QUOTES };
enum class CorporateActionType {
    REVERSE_SPLIT, FORWARD_SPLIT, UNIT_SPLIT,
    CASH_DIVIDEND, STOCK_DIVIDEND, SPIN_OFF,
    CASH_MERGER, STOCK_MERGER, STOCK_AND_CASH_MERGER,
    REDEMPTION, NAME_CHANGE, WORTHLESS_REMOVAL,
    RIGHTS_DISTRIBUTION, CONTRACT_ADJUSTMENT
};

} // namespace alpaca