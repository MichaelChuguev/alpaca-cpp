#pragma once

#include <chrono>
#include <date/date.h>
#include <string>
#include <optional>

#include "nlohmann/json.hpp"

namespace alpaca {

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
    
    // Comparisons
    bool operator<(const DateTime&) const noexcept;
    bool operator==(const DateTime&) const noexcept;
    bool operator>(const DateTime&) const noexcept;
    
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

// Common type aliases
using Decimal = double; // Or use a fixed-point decimal class
using Timestamp = DateTime;



enum TraderAPIEndpoint {
    PAPER,
    LIVE
};

enum MarketDataWebsocketSource {
    IEX,
    SIP
};

enum BrokerAPIEndpoint {
    SANDBOX,
    PRODUCTION
};

enum Sort {
    ASC,
    DESC
};

enum AssetClass {
    US_EQUITY,
    US_OPTION,
    CRYPTO
};

enum Exchange {
    AMEX,
    ARCA,
    BATS,
    NYSE,
    NASDAQ,
    NYSEARCA,
    OTC
};

} // namespace alpaca