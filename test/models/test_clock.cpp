#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/clock.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_clock_json(bool is_open = true) {
    json j;
    j["timestamp"] = "2024-01-15T14:30:00-05:00";
    j["is_open"] = is_open;
    j["next_open"] = "2024-01-16T09:30:00-05:00";
    j["next_close"] = "2024-01-15T16:00:00-05:00";
    return j;
}

} // namespace

TEST(ClockTest, ParseFromJson) {
    auto j = make_clock_json(true);
    Clock clock(j);

    EXPECT_TRUE(clock.is_open);
}

TEST(ClockTest, ParseClosedMarket) {
    auto j = make_clock_json(false);
    Clock clock(j);

    EXPECT_FALSE(clock.is_open);
}

TEST(ClockTest, ToString) {
    auto j = make_clock_json(true);
    Clock clock(j);
    std::string str = clock.to_string();

    EXPECT_NE(str.find("Clock:"), std::string::npos);
    EXPECT_NE(str.find("Is Open:"), std::string::npos);
}

TEST(ClockTest, ToJson) {
    auto j = make_clock_json(true);
    Clock clock(j);
    json result = clock.to_json();

    EXPECT_TRUE(result["is_open"].get<bool>());
    EXPECT_TRUE(result.contains("timestamp"));
    EXPECT_TRUE(result.contains("next_open"));
    EXPECT_TRUE(result.contains("next_close"));
}

TEST(ClockTest, RoundTripIsOpen) {
    auto j = make_clock_json(false);
    Clock clock(j);
    json result = clock.to_json();

    EXPECT_FALSE(result["is_open"].get<bool>());
}
