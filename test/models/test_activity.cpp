#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/activity.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_trade_activity_json() {
    json j;
    j["activity_type"] = "FILL";
    j["id"] = "act-trade-1";
    j["symbol"] = "AAPL";
    j["qty"] = "10";
    j["cum_qty"] = "10";
    j["leaves_qty"] = "0";
    j["price"] = "150.25";
    j["side"] = "buy";
    j["transaction_time"] = "2024-01-15T14:30:00Z";
    j["order_id"] = "order-123";
    j["type"] = "fill";
    j["order_status"] = "filled";
    return j;
}

json make_non_trade_activity_json() {
    json j;
    j["activity_type"] = "DIV";
    j["id"] = "act-nontrade-1";
    j["symbol"] = "AAPL";
    j["qty"] = "100";
    j["activity_sub_type"] = "DIV";
    j["date"] = "2024-01-15";
    j["net_amount"] = "24.50";
    j["per_share_amount"] = "0.245";
    j["description"] = "Dividend payment";
    j["status"] = "executed";
    return j;
}

json make_other_activity_json() {
    json j;
    j["activity_type"] = "MISC";
    j["id"] = "act-other-1";
    j["qty"] = "0";
    return j;
}

} // namespace

// ===== Trade Activity Tests =====

TEST(AccountActivityTest, ParseTradeActivity) {
    auto j = make_trade_activity_json();
    AccountActivity act(j);

    EXPECT_EQ(act.account_activity_type, "trade_activity");
    EXPECT_EQ(act.activity_type, "FILL");
    EXPECT_EQ(act.id, "act-trade-1");
    EXPECT_EQ(act.symbol, "AAPL");
    EXPECT_DOUBLE_EQ(act.qty, 10.0);
    EXPECT_DOUBLE_EQ(act.cum_qty, 10.0);
    EXPECT_DOUBLE_EQ(act.leaves_qty, 0.0);
    EXPECT_DOUBLE_EQ(act.price, 150.25);
    EXPECT_EQ(act.side, "buy");
    EXPECT_EQ(act.transaction_time, "2024-01-15T14:30:00Z");
    EXPECT_EQ(act.order_id, "order-123");
    EXPECT_EQ(act.type, "fill");
    EXPECT_EQ(act.order_status, "filled");
}

TEST(AccountActivityTest, TradeActivityToString) {
    auto j = make_trade_activity_json();
    AccountActivity act(j);
    std::string str = act.to_string();

    EXPECT_NE(str.find("Account Activity:"), std::string::npos);
    EXPECT_NE(str.find("trade_activity"), std::string::npos);
    EXPECT_NE(str.find("AAPL"), std::string::npos);
    EXPECT_NE(str.find("order-123"), std::string::npos);
}

TEST(AccountActivityTest, TradeActivityToJson) {
    auto j = make_trade_activity_json();
    AccountActivity act(j);
    json result = act.to_json();

    EXPECT_EQ(result["account_activity_type"], "trade_activity");
    EXPECT_EQ(result["activity_type"], "FILL");
    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_EQ(result["order_id"], "order-123");
    EXPECT_EQ(result["side"], "buy");
    EXPECT_DOUBLE_EQ(result["cum_qty"].get<double>(), 10.0);
    EXPECT_DOUBLE_EQ(result["price"].get<double>(), 150.25);
}

// ===== Non-Trade Activity Tests =====

TEST(AccountActivityTest, ParseNonTradeActivity) {
    auto j = make_non_trade_activity_json();
    AccountActivity act(j);

    EXPECT_EQ(act.account_activity_type, "non_trade_activity");
    EXPECT_EQ(act.activity_type, "DIV");
    EXPECT_EQ(act.id, "act-nontrade-1");
    EXPECT_EQ(act.symbol, "AAPL");
    EXPECT_DOUBLE_EQ(act.qty, 100.0);
    EXPECT_EQ(act.activity_sub_type, "DIV");
    EXPECT_DOUBLE_EQ(act.net_amount, 24.50);
    EXPECT_DOUBLE_EQ(act.per_share_amount, 0.245);
    EXPECT_EQ(act.description, "Dividend payment");
    EXPECT_EQ(act.status, "executed");
}

TEST(AccountActivityTest, NonTradeActivityToString) {
    auto j = make_non_trade_activity_json();
    AccountActivity act(j);
    std::string str = act.to_string();

    EXPECT_NE(str.find("non_trade_activity"), std::string::npos);
    EXPECT_NE(str.find("Dividend payment"), std::string::npos);
}

TEST(AccountActivityTest, NonTradeActivityToJson) {
    auto j = make_non_trade_activity_json();
    AccountActivity act(j);
    json result = act.to_json();

    EXPECT_EQ(result["account_activity_type"], "non_trade_activity");
    EXPECT_EQ(result["activity_sub_type"], "DIV");
    EXPECT_DOUBLE_EQ(result["net_amount"].get<double>(), 24.50);
    EXPECT_DOUBLE_EQ(result["per_share_amount"].get<double>(), 0.245);
    EXPECT_EQ(result["description"], "Dividend payment");
    EXPECT_EQ(result["status"], "executed");
}

// ===== Other Activity Tests =====

TEST(AccountActivityTest, ParseOtherActivity) {
    auto j = make_other_activity_json();
    AccountActivity act(j);

    EXPECT_EQ(act.account_activity_type, "other_activity");
    EXPECT_EQ(act.activity_type, "MISC");
    EXPECT_EQ(act.id, "act-other-1");
}

// ===== Array Parsing =====

TEST(AccountActivityTest, ParseMultipleActivities) {
    json arr = json::array();
    arr.push_back(make_trade_activity_json());
    arr.push_back(make_non_trade_activity_json());
    arr.push_back(make_other_activity_json());

    std::vector<AccountActivity> activities;
    for (const auto& a : arr) {
        activities.push_back(AccountActivity(a));
    }

    ASSERT_EQ(activities.size(), 3u);
    EXPECT_EQ(activities[0].account_activity_type, "trade_activity");
    EXPECT_EQ(activities[1].account_activity_type, "non_trade_activity");
    EXPECT_EQ(activities[2].account_activity_type, "other_activity");
}
