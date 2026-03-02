#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/calendar.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_single_calendar_day_json() {
    json j;
    j["date"] = "2024-01-15";
    j["open"] = "09:30";
    j["close"] = "16:00";
    j["session_open"] = "04:00";
    j["session_close"] = "20:00";
    j["settlement_date"] = "2024-01-17";
    return j;
}

json make_calendar_json() {
    json arr = json::array();
    arr.push_back(make_single_calendar_day_json());

    json day2;
    day2["date"] = "2024-01-16";
    day2["open"] = "09:30";
    day2["close"] = "16:00";
    day2["session_open"] = "04:00";
    day2["session_close"] = "20:00";
    day2["settlement_date"] = "2024-01-18";
    arr.push_back(day2);

    return arr;
}

} // namespace

TEST(SingleCalendarDayTest, ParseFromJson) {
    auto j = make_single_calendar_day_json();
    Calendar::SingleCalendarDay day(j);

    EXPECT_EQ(day.open, "09:30");
    EXPECT_EQ(day.close, "16:00");
    EXPECT_EQ(day.session_open, "04:00");
    EXPECT_EQ(day.session_close, "20:00");
}

TEST(SingleCalendarDayTest, ToString) {
    auto j = make_single_calendar_day_json();
    Calendar::SingleCalendarDay day(j);
    std::string str = day.to_string();

    EXPECT_NE(str.find("Calendar Day:"), std::string::npos);
    EXPECT_NE(str.find("09:30"), std::string::npos);
    EXPECT_NE(str.find("16:00"), std::string::npos);
}

TEST(CalendarTest, ParseFromJsonArray) {
    auto arr = make_calendar_json();
    Calendar cal(arr);

    ASSERT_EQ(cal.calendarDays.size(), 2u);
    EXPECT_EQ(cal.calendarDays[0].open, "09:30");
    EXPECT_EQ(cal.calendarDays[0].close, "16:00");
    EXPECT_EQ(cal.calendarDays[1].open, "09:30");
}

TEST(CalendarTest, SingleDay) {
    json arr = json::array();
    arr.push_back(make_single_calendar_day_json());

    Calendar cal(arr);
    ASSERT_EQ(cal.calendarDays.size(), 1u);
}

TEST(CalendarTest, EmptyCalendar) {
    json arr = json::array();
    Calendar cal(arr);

    EXPECT_EQ(cal.calendarDays.size(), 0u);
}

TEST(CalendarTest, ToString) {
    auto arr = make_calendar_json();
    Calendar cal(arr);
    std::string str = cal.to_string();

    EXPECT_NE(str.find("Calendar Day:"), std::string::npos);
}
