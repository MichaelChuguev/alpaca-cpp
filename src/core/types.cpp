#include <compare>
#include <iomanip>
#include <sstream>
#include <charconv>

#include "alpaca/core/types.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Decimal
// ---------------------------------------------------------------------------

Decimal::Decimal(const std::string& str) {
    if (str.empty()) return;
    bool negative = false;
    size_t i = 0;
    if (str[0] == '-') { negative = true; ++i; }
    else if (str[0] == '+') { ++i; }

    int64_t whole = 0;
    while (i < str.size() && str[i] != '.') {
        whole = whole * 10 + (str[i] - '0');
        ++i;
    }

    int64_t frac = 0;
    int frac_digits = 0;
    if (i < str.size() && str[i] == '.') {
        ++i;
        while (i < str.size() && frac_digits < SCALE) {
            frac = frac * 10 + (str[i] - '0');
            ++frac_digits;
            ++i;
        }
    }
    while (frac_digits < SCALE) {
        frac *= 10;
        ++frac_digits;
    }

    mantissa_ = whole * SCALE_FACTOR + frac;
    if (negative) mantissa_ = -mantissa_;
}

std::string Decimal::to_string() const {
    if (mantissa_ == 0) return "0";

    bool negative = mantissa_ < 0;
    int64_t abs_m = negative ? -mantissa_ : mantissa_;
    int64_t whole = abs_m / SCALE_FACTOR;
    int64_t frac = abs_m % SCALE_FACTOR;

    std::string result;
    if (negative) result += '-';
    result += std::to_string(whole);
    if (frac > 0) {
        result += '.';
        std::string frac_str = std::to_string(frac);
        result += std::string(static_cast<size_t>(SCALE) - frac_str.size(), '0');
        result += frac_str;
        while (result.back() == '0') result.pop_back();
    }

    return result;
}

Decimal Decimal::operator*(const Decimal& rhs) const noexcept {
    return Decimal(to_double() * rhs.to_double());
}

Decimal Decimal::operator/(const Decimal& rhs) const noexcept {
    if (rhs.is_zero()) return Decimal{};
    return Decimal(to_double() / rhs.to_double());
}

// ---------------------------------------------------------------------------
// DateTime
// ---------------------------------------------------------------------------

DateTime::DateTime(const std::string& iso_str) {
    if (std::all_of(iso_str.begin(), iso_str.end(), ::isdigit)) {
        int64_t epoch_seconds;
        std::istringstream(iso_str) >> epoch_seconds;
        time_point_ = std::chrono::system_clock::time_point(
            std::chrono::seconds(epoch_seconds));
        offset_minutes_ = std::nullopt;
    }
    else if (iso_str.find('T') != std::string::npos) {
        parse_iso_datetime(iso_str, time_point_, offset_minutes_);
    }
    else {
        parse_iso_date(iso_str, time_point_);
    }
}

DateTime::DateTime(sys_time tp, std::optional<int> offset_minutes)
    : time_point_(tp),
      offset_minutes_(offset_minutes) 
{
    
    if (offset_minutes) {
        const int offset = *offset_minutes;
        if (offset < -840 || offset > 840) {
            throw std::invalid_argument("UTC offset must be between -840 and +840 minutes");
        }
    }
}

DateTime::DateTime(int64_t unix_epoch_seconds) 
: 
    time_point_(std::chrono::system_clock::time_point(std::chrono::seconds(unix_epoch_seconds))),
    offset_minutes_(std::nullopt)
{
}

DateTime DateTime::parse(const nlohmann::json& j) {
    if (j.is_string()) {
        return DateTime(j.get<std::string>()); 
    } else if (j.is_number()) {
        return DateTime(j.get<int64_t>());
    }
    throw std::runtime_error("Invalid JSON format for DateTime");
}

DateTime DateTime::now() {
    sys_time now = std::chrono::system_clock::now();
    
    return DateTime(now);
}

void DateTime::parse_iso_datetime(const std::string& str, sys_time& tp, std::optional<int>& offset) {
    std::istringstream iss(str);
    std::chrono::sys_days days;
    std::chrono::microseconds us{0};
    char sep;

    // Parse date (YYYY-MM-DD)
    iss >> date::parse("%F", days) >> sep;  // %F = YYYY-MM-DD

    // Parse time (HH:MM:SS.FFFFFF)
    if (sep == 'T') {
        int h, m, s = 0;
        char colon1, colon2;

        iss >> h >> colon1 >> m >> colon2 >> s;
        
        // Parse fractional seconds if present
        if (iss.peek() == '.') {
            iss.ignore(); // skip the '.'
            std::string subsec_str;
            while (std::isdigit(iss.peek())) {
                subsec_str += iss.get();
            }
            // Convert fractional string to microseconds
            // "123456" -> 0.123456 seconds -> 123456 microseconds
            if (!subsec_str.empty()) {
                double fraction = std::stod("0." + subsec_str);
                us = std::chrono::hours(h) + 
                     std::chrono::minutes(m) + 
                     std::chrono::seconds(s) +
                     std::chrono::round<std::chrono::microseconds>(
                         std::chrono::duration<double>(fraction));
            } else {
                us = std::chrono::hours(h) + 
                     std::chrono::minutes(m) + 
                     std::chrono::seconds(s);
            }
        } else {
            us = std::chrono::hours(h) + 
                 std::chrono::minutes(m) + 
                 std::chrono::seconds(s);
        }
    }

    // Parse timezone (Z or ±HH:MM)
    if (iss.peek() == 'Z') {
        iss.ignore();
        offset = 0;
    } else if (iss.peek() == '+' || iss.peek() == '-') {
        char sign;
        int offset_hours, offset_minutes = 0;
        iss >> sign >> offset_hours;
        if (iss.peek() == ':') {
            iss.ignore() >> offset_minutes;
        }
        offset = (sign == '-' ? -1 : 1) * (offset_hours * 60 + offset_minutes);
    }

    tp = days + us;
}

void DateTime::parse_iso_date(const std::string& str, sys_time& tp) {
    std::istringstream iss(str);
    std::chrono::sys_days days; // Represents a date (days since epoch)

    if (!(iss >> date::parse("%F", days))) { // "%F" = "%Y-%m-%d"
        throw std::runtime_error("Failed to parse date: " + str);
    }
    tp = days; // Converts sys_days to sys_time
}

std::string DateTime::to_iso_string(bool as_epoch) const {
    std::ostringstream oss;
    auto days = std::chrono::floor<std::chrono::days>(time_point_);

    if (as_epoch) {
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
            time_point_.time_since_epoch());
        return std::to_string(seconds.count());
    }
    
    // Date portion
    oss << std::chrono::year_month_day(days);
    
    // Time portion if not date-only
    if (!is_date_only()) {
        auto time = time_point_ - days;
        auto h = std::chrono::duration_cast<std::chrono::hours>(time);
        auto m = std::chrono::duration_cast<std::chrono::minutes>(time - h);
        auto s = std::chrono::duration_cast<std::chrono::seconds>(time - h - m);
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(time - h - m - s);
        
        oss << 'T' << std::setfill('0')
            << std::setw(2) << h.count() << ':'
            << std::setw(2) << m.count() << ':'
            << std::setw(2) << s.count();
            
        if (us.count() > 0) {
            oss << '.' << std::setw(6) << us.count();
        }
        
        // Timezone
        if (offset_minutes_) {
            int offset = *offset_minutes_;
            char sign = offset >= 0 ? '+' : '-';
            oss << sign 
                << std::setw(2) << std::abs(offset)/60 << ':'
                << std::setw(2) << std::abs(offset)%60;
        } else {
            oss << 'Z';
        }
    }
    
    return oss.str();
}

int64_t DateTime::to_epoch_seconds() const noexcept {
    return std::chrono::duration_cast<std::chrono::seconds>(time_point_.time_since_epoch()).count();
}

DateTime::sys_time DateTime::to_time_point() const noexcept {
    return time_point_;
}

std::optional<int> DateTime::utc_offset_minutes() const noexcept {
    return offset_minutes_;
}

std::strong_ordering DateTime::operator<=>(const DateTime& other) const noexcept {
    if (time_point_ < other.time_point_) return std::strong_ordering::less;
    if (time_point_ > other.time_point_) return std::strong_ordering::greater;
    return std::strong_ordering::equal;
}

bool DateTime::operator==(const DateTime& other) const noexcept {
    return time_point_ == other.time_point_;
}

bool DateTime::is_date_only() const noexcept {
    auto time = time_point_ - std::chrono::floor<std::chrono::days>(time_point_);
    return time == std::chrono::seconds(0);
}

} // namespace alpaca