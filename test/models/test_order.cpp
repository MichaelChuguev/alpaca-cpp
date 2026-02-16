#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/order.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

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

TEST(OrderResponseTest, ParseFromJson) {
    json j = make_order_response_json();
    OrderResponse order(j);

    EXPECT_EQ(order.id, "61e69015-8549-4bab-b63c-cc230c547b33");
    EXPECT_EQ(order.client_order_id, "eb9e2aaa-f71a-4f51-b5b4-52a6c565dad4");
    EXPECT_EQ(order.asset_id, "b0b6dd9d-8b9b-48a9-ba46-b9d54906e415");
    EXPECT_EQ(order.symbol, "AAPL");
    EXPECT_EQ(order.asset_class, AssetClass::US_EQUITY);
    EXPECT_DOUBLE_EQ(order.qty, 1.0);
    EXPECT_EQ(order.order_class, OrderClass::SIMPLE);
    EXPECT_EQ(order.type, OrderType::MARKET);
    EXPECT_EQ(order.side, OrderSide::BUY);
    EXPECT_EQ(order.position_intent, PositionIntent::BUY_TO_OPEN);
    EXPECT_EQ(order.time_in_force, OrderTimeInForce::DAY);
    EXPECT_EQ(order.status, "new");
    EXPECT_FALSE(order.extended_hours);
}

TEST(OrderResponseTest, ParseWithLegs) {
    json j = make_order_response_json();
    j["legs"] = json::array({make_order_response_json()});
    OrderResponse order(j);

    ASSERT_EQ(order.legs.size(), 1u);
    EXPECT_EQ(order.legs[0].symbol, "AAPL");
}

TEST(OrderResponseTest, ParseDifferentOrderTypes) {
    json j = make_order_response_json();
    
    j["type"] = "limit";
    OrderResponse limit_order(j);
    EXPECT_EQ(limit_order.type, OrderType::LIMIT);

    j["type"] = "stop";
    OrderResponse stop_order(j);
    EXPECT_EQ(stop_order.type, OrderType::STOP);

    j["type"] = "stop_limit";
    OrderResponse stop_limit_order(j);
    EXPECT_EQ(stop_limit_order.type, OrderType::STOP_LIMIT);

    j["type"] = "trailing_stop";
    OrderResponse trailing_order(j);
    EXPECT_EQ(trailing_order.type, OrderType::TRAILING_STOP);
}

TEST(OrderResponseTest, ParseDifferentSides) {
    json j = make_order_response_json();
    
    j["side"] = "sell";
    OrderResponse sell_order(j);
    EXPECT_EQ(sell_order.side, OrderSide::SELL);
}

TEST(OrderResponseTest, ParseDifferentTimeInForce) {
    json j = make_order_response_json();
    
    j["time_in_force"] = "gtc";
    OrderResponse gtc_order(j);
    EXPECT_EQ(gtc_order.time_in_force, OrderTimeInForce::GTC);

    j["time_in_force"] = "ioc";
    OrderResponse ioc_order(j);
    EXPECT_EQ(ioc_order.time_in_force, OrderTimeInForce::IOC);

    j["time_in_force"] = "fok";
    OrderResponse fok_order(j);
    EXPECT_EQ(fok_order.time_in_force, OrderTimeInForce::FOK);
}

TEST(OrderResponseTest, ParseDifferentOrderClasses) {
    json j = make_order_response_json();

    j["order_class"] = "oco";
    OrderResponse oco_order(j);
    EXPECT_EQ(oco_order.order_class, OrderClass::OCO);

    j["order_class"] = "bracket";
    OrderResponse bracket_order(j);
    EXPECT_EQ(bracket_order.order_class, OrderClass::BRACKET);

    j["order_class"] = "mleg";
    OrderResponse mleg_order(j);
    EXPECT_EQ(mleg_order.order_class, OrderClass::MLEG);
}

TEST(OrderResponseTest, ParseDifferentAssetClasses) {
    json j = make_order_response_json();

    j["asset_class"] = "us_option";
    OrderResponse option_order(j);
    EXPECT_EQ(option_order.asset_class, AssetClass::US_OPTION);

    j["asset_class"] = "crypto";
    OrderResponse crypto_order(j);
    EXPECT_EQ(crypto_order.asset_class, AssetClass::CRYPTO);
}

TEST(OrderResponseTest, ToJson) {
    json j = make_order_response_json();
    OrderResponse order(j);
    json result = order.to_json();

    EXPECT_EQ(result["id"], "61e69015-8549-4bab-b63c-cc230c547b33");
    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_EQ(result["type"], "market");
    EXPECT_EQ(result["side"], "buy");
    EXPECT_EQ(result["status"], "new");
}

TEST(OrderResponseTest, ToString) {
    json j = make_order_response_json();
    OrderResponse order(j);
    std::string str = order.to_string();

    EXPECT_NE(str.find("Order Status:"), std::string::npos);
    EXPECT_NE(str.find("AAPL"), std::string::npos);
}

TEST(OrderTest, ToJson) {
    Order order("AAPL", 1.0, OrderSide::BUY, OrderType::MARKET, OrderTimeInForce::DAY);
    json result = order.to_json();

    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_EQ(result["side"], "buy");
    EXPECT_EQ(result["type"], "market");
    EXPECT_EQ(result["time_in_force"], "day");
}

TEST(LegTest, ToJson) {
    Leg leg("AAPL", OrderSide::BUY, 1.0, PositionIntent::BUY_TO_OPEN);
    json result = leg.to_json();

    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_EQ(result["side"], "BUY");
    EXPECT_DOUBLE_EQ(result["ratio_qty"].get<double>(), 1.0);
}

TEST(LegTest, ToString) {
    Leg leg("AAPL", OrderSide::BUY, 1.0, PositionIntent::BUY_TO_OPEN);
    std::string str = leg.to_string();

    EXPECT_NE(str.find("AAPL"), std::string::npos);
    EXPECT_NE(str.find("BUY"), std::string::npos);
}
