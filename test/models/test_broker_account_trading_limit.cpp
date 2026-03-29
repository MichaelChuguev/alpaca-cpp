#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>

#include "nlohmann/json.hpp"
#include "alpaca/AlpacaBrokerAPI.h"
#include "alpaca/model/broker/brokerAccountTradingLimit.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_broker_account_trading_limit_json() {
    json j;
    j["daily_net_limit"] = "100000.50";
    j["available"] = "76543.21";
    j["used"] = "12345.67";
    j["held"] = "11111.62";
    j["swap_rate"] = "1.0845";
    j["usd"] = {
        {"daily_net_limit", "92000.00"},
        {"available", "70000.00"},
        {"used", "12000.00"},
        {"held", "10000.00"}
    };
    return j;
}

} // namespace

static_assert(
    std::is_same_v<
        decltype(&AlpacaBrokerAPI::get_account_trading_limits),
        BrokerAccountTradingLimit (AlpacaBrokerAPI::*)(const std::string&)
    >,
    "Broker API trading-limits method signature changed unexpectedly");

TEST(BrokerAccountTradingLimitTest, ParseFromJson) {
    std::cout << "\n=== Test: BrokerAccountTradingLimitTest::ParseFromJson ===" << std::endl;

    const BrokerAccountTradingLimit limit(make_broker_account_trading_limit_json());

    EXPECT_EQ(limit.daily_net_limit, Decimal(100000.50));
    EXPECT_EQ(limit.available, Decimal(76543.21));
    EXPECT_EQ(limit.used, Decimal(12345.67));
    EXPECT_EQ(limit.held, Decimal(11111.62));
    EXPECT_EQ(limit.swap_rate, Decimal(1.0845));
    EXPECT_EQ(limit.usd.daily_net_limit, Decimal(92000.00));
    EXPECT_EQ(limit.usd.available, Decimal(70000.00));
    EXPECT_EQ(limit.usd.used, Decimal(12000.00));
    EXPECT_EQ(limit.usd.held, Decimal(10000.00));
}

TEST(BrokerAccountTradingLimitTest, ToJson) {
    std::cout << "\n=== Test: BrokerAccountTradingLimitTest::ToJson ===" << std::endl;

    const BrokerAccountTradingLimit limit(make_broker_account_trading_limit_json());
    const json result = limit.to_json();

    EXPECT_EQ(result["daily_net_limit"], "100000.5");
    EXPECT_EQ(result["available"], "76543.21");
    EXPECT_EQ(result["used"], "12345.67");
    EXPECT_EQ(result["held"], "11111.62");
    EXPECT_EQ(result["swap_rate"], "1.0845");
    EXPECT_EQ(result["usd"]["daily_net_limit"], "92000");
    EXPECT_EQ(result["usd"]["available"], "70000");
    EXPECT_EQ(result["usd"]["used"], "12000");
    EXPECT_EQ(result["usd"]["held"], "10000");
}

TEST(BrokerAccountTradingLimitTest, ToString) {
    std::cout << "\n=== Test: BrokerAccountTradingLimitTest::ToString ===" << std::endl;

    const BrokerAccountTradingLimit limit(make_broker_account_trading_limit_json());
    const std::string str = limit.to_string();

    EXPECT_NE(str.find("daily_net_limit"), std::string::npos);
    EXPECT_NE(str.find("swap_rate"), std::string::npos);
    EXPECT_NE(str.find("usd"), std::string::npos);
}
