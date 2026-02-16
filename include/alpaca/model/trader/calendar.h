#pragma once

#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

struct Calendar {

    struct SingleCalendarDay {

        DateTime date;
        std::string open;
        std::string close;
        std::string session_open;
        std::string session_close;
        DateTime settlement_date;

        SingleCalendarDay(const json& j)
        : 
            date(DateTime::parse(j["date"])),
            open(j["open"].get<std::string>()),
            close(j["close"].get<std::string>()),
            session_open(j["session_open"].get<std::string>()),
            session_close(j["session_close"].get<std::string>()),
            settlement_date(DateTime::parse(j["settlement_date"])) 
        {
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << "Calendar Day:\n"
                << "Date: " << date.to_string() << "\n"
                << "Open: " << open << "\n"
                << "Close: " << close << "\n"
                << "Session Open: " << session_open << "\n"
                << "Session Close: " << session_close << "\n"
                << "Settlement Date: " << settlement_date.to_string() << "\n";

            return oss.str();
        }
    };

    std::vector<SingleCalendarDay> calendarDays;

    Calendar(const json& j) {
        for (const auto& item : j) {
            SingleCalendarDay day(item);
            calendarDays.push_back(day);
        }
    }

    std::string toString() const {
        std::ostringstream oss;
        for (const auto& day : calendarDays) oss << day.toString() << "\n";
        return oss.str();
    }
};

} // namespace alpaca