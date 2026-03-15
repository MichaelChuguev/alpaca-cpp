#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/position.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_position_json() {
    json j;
    j["asset_id"] = "904837e3-3b76-47ec-b432-046db621571b";
    j["symbol"] = "AAPL";
    j["exchange"] = "NASDAQ";
    j["asset_class"] = "us_equity";
    j["avg_entry_price"] = 150.0;
    j["qty"] = 10.0;
    j["qty_available"] = 10.0;
    j["side"] = "buy";
    j["market_value"] = 1550.0;
    j["cost_basis"] = 1500.0;
    j["unrealized_pl"] = 50.0;
    j["unrealized_plpc"] = 0.033;
    j["unrealized_intraday_pl"] = 25.0;
    j["unrealized_intraday_plpc"] = 0.016;
    j["current_price"] = 155.0;
    j["lastday_price"] = 152.0;
    j["change_today"] = 0.02;
    j["asset_marginable"] = true;
    return j;
}

json make_order_response_json() {
    json j;
    j["id"] = "61e69015-8549-4bab-b63c-cc230c547b33";
    j["client_order_id"] = "eb9e2aaa-f71a-4f51-b5b4-52a6c565dad4";
    j["created_at"] = "2021-03-16T18:38:01.942282Z";
    j["updated_at"] = "2021-03-16T18:38:01.942282Z";
    j["submitted_at"] = "2021-03-16T18:38:01.937734Z";
    j["filled_at"] = "2021-03-16T18:38:01.937734Z";
    j["expired_at"] = "2021-03-16T18:38:01.937734Z";
    j["canceled_at"] = "2021-03-16T18:38:01.937734Z";
    j["failed_at"] = "2021-03-16T18:38:01.937734Z";
    j["replaced_at"] = "2021-03-16T18:38:01.937734Z";
    j["replaced_by"] = "";
    j["replaces"] = "";
    j["asset_id"] = "b0b6dd9d-8b9b-48a9-ba46-b9d54906e415";
    j["symbol"] = "AAPL";
    j["asset_class"] = "us_equity";
    j["notional"] = 0.0;
    j["qty"] = 1.0;
    j["filled_qty"] = 0.0;
    j["filled_avg_price"] = 0.0;
    j["order_class"] = "simple";
    j["type"] = "market";
    j["side"] = "buy";
    j["position_intent"] = "buy_to_open";
    j["time_in_force"] = "day";
    j["limit_price"] = 0.0;
    j["stop_price"] = 0.0;
    j["status"] = "new";
    j["extended_hours"] = false;
    j["trail_percent"] = 0.0;
    j["trail_price"] = 0.0;
    j["hwm"] = 0.0;
    return j;
}

} // anonymous namespace

TEST(PositionTest, ParseFromJson) {
    json j = make_position_json();
    Position pos(j);

    EXPECT_EQ(pos.asset_id, "904837e3-3b76-47ec-b432-046db621571b");
    EXPECT_EQ(pos.symbol, "AAPL");
    EXPECT_EQ(pos.exchange, Exchange::NASDAQ);
    EXPECT_EQ(pos.asset_class, AssetClass::US_EQUITY);
    EXPECT_EQ(pos.avg_entry_price, Decimal(150.0));
    EXPECT_EQ(pos.qty, Decimal(10.0));
    EXPECT_EQ(pos.qty_available, Decimal(10.0));
    EXPECT_EQ(pos.side, OrderSide::BUY);
    EXPECT_EQ(pos.market_value, Decimal(1550.0));
    EXPECT_EQ(pos.cost_basis, Decimal(1500.0));
    EXPECT_EQ(pos.unrealized_pl, Decimal(50.0));
    EXPECT_EQ(pos.current_price, Decimal(155.0));
    EXPECT_EQ(pos.lastday_price, Decimal(152.0));
    EXPECT_TRUE(pos.asset_marginable);
}

TEST(PositionTest, ParseDifferentExchanges) {
    json j = make_position_json();

    j["exchange"] = "NYSE";
    Position nyse_pos(j);
    EXPECT_EQ(nyse_pos.exchange, Exchange::NYSE);

    j["exchange"] = "AMEX";
    Position amex_pos(j);
    EXPECT_EQ(amex_pos.exchange, Exchange::AMEX);

    j["exchange"] = "BATS";
    Position bats_pos(j);
    EXPECT_EQ(bats_pos.exchange, Exchange::BATS);

    j["exchange"] = "NYSEARCA";
    Position arca_pos(j);
    EXPECT_EQ(arca_pos.exchange, Exchange::NYSEARCA);

    j["exchange"] = "OTC";
    Position otc_pos(j);
    EXPECT_EQ(otc_pos.exchange, Exchange::OTC);
}

TEST(PositionTest, ParseDifferentAssetClasses) {
    json j = make_position_json();

    j["asset_class"] = "us_option";
    Position opt_pos(j);
    EXPECT_EQ(opt_pos.asset_class, AssetClass::US_OPTION);

    j["asset_class"] = "crypto";
    Position crypto_pos(j);
    EXPECT_EQ(crypto_pos.asset_class, AssetClass::CRYPTO);
}

TEST(PositionTest, ToJson) {
    json j = make_position_json();
    Position pos(j);
    json result = pos.to_json();

    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_EQ(result["exchange"], "NASDAQ");
    EXPECT_EQ(result["asset_class"], "us_equity");
    EXPECT_EQ(result["side"], "buy");
    EXPECT_DOUBLE_EQ(result["current_price"].get<double>(), 155.0);
}

TEST(PositionTest, ToString) {
    json j = make_position_json();
    Position pos(j);
    std::string str = pos.to_string();

    EXPECT_NE(str.find("Position: AAPL"), std::string::npos);
    EXPECT_NE(str.find("NASDAQ"), std::string::npos);
}

TEST(PositionClosedTest, ParseFromJson) {
    json j;
    j["symbol"] = "AAPL";
    j["status"] = 200;
    j["body"] = make_order_response_json();

    PositionClosed closed(j);

    EXPECT_EQ(closed.symbol, "AAPL");
    EXPECT_EQ(closed.status, 200);
    EXPECT_EQ(closed.body.symbol, "AAPL");
}

TEST(PositionClosedTest, ToJson) {
    json j;
    j["symbol"] = "AAPL";
    j["status"] = 200;
    j["body"] = make_order_response_json();

    PositionClosed closed(j);
    json result = closed.to_json();

    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_EQ(result["status"], 200);
}

TEST(PositionClosedTest, ToString) {
    json j;
    j["symbol"] = "AAPL";
    j["status"] = 200;
    j["body"] = make_order_response_json();

    PositionClosed closed(j);
    std::string str = closed.to_string();

    EXPECT_NE(str.find("PositionClosed: AAPL"), std::string::npos);
}
