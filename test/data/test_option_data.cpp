#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/model/data/option_data.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_option_trade_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["p"] = "5.75";
    j["s"] = "10";
    j["x"] = "C";
    j["c"] = "I";
    j["S"] = "AAPL250321C00200000";
    return j;
}

json make_option_quote_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["ap"] = "5.90";
    j["as"] = "50";
    j["ax"] = "C";
    j["bp"] = "5.60";
    j["bs"] = "40";
    j["bx"] = "B";
    j["c"] = "A";
    j["S"] = "AAPL250321C00200000";
    return j;
}

json make_option_bar_json() {
    json j;
    j["t"] = "2026-03-10T00:00:00Z";
    j["o"] = "5.50";
    j["h"] = "6.00";
    j["l"] = "5.30";
    j["c"] = "5.75";
    j["v"] = "15000";
    j["n"] = 850;
    j["vw"] = "5.65";
    j["S"] = "AAPL250321C00200000";
    return j;
}

json make_option_snapshot_json() {
    json j;
    j["latestTrade"] = make_option_trade_json();
    j["latestQuote"] = make_option_quote_json();
    j["impliedVolatility"] = "0.35";
    json greeks;
    greeks["delta"] = "0.65";
    greeks["gamma"] = "0.03";
    greeks["theta"] = "-0.05";
    greeks["vega"] = "0.12";
    greeks["rho"] = "0.08";
    j["greeks"] = greeks;
    return j;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// OptionTrade tests
// ---------------------------------------------------------------------------

TEST(OptionTradeTest, ParseFromJson) {
    std::cout << "\n=== Test: OptionTradeTest::ParseFromJson ===" << std::endl;

    json j = make_option_trade_json();
    OptionTrade trade(j);

    EXPECT_EQ(trade.price, Decimal(5.75));
    EXPECT_EQ(trade.size, Decimal(10));
    EXPECT_EQ(trade.exchange, "C");
    EXPECT_EQ(trade.condition, "I");
    EXPECT_EQ(trade.symbol, "AAPL250321C00200000");

    std::cout << "PASSED" << std::endl;
}

TEST(OptionTradeTest, DefaultConstruct) {
    OptionTrade trade;
    EXPECT_EQ(trade.price, Decimal());
    EXPECT_TRUE(trade.exchange.empty());
}

// ---------------------------------------------------------------------------
// OptionQuote tests
// ---------------------------------------------------------------------------

TEST(OptionQuoteTest, ParseFromJson) {
    std::cout << "\n=== Test: OptionQuoteTest::ParseFromJson ===" << std::endl;

    json j = make_option_quote_json();
    OptionQuote quote(j);

    EXPECT_EQ(quote.ask_price, Decimal(5.90));
    EXPECT_EQ(quote.ask_size, Decimal(50));
    EXPECT_EQ(quote.ask_exchange, "C");
    EXPECT_EQ(quote.bid_price, Decimal(5.60));
    EXPECT_EQ(quote.bid_size, Decimal(40));
    EXPECT_EQ(quote.bid_exchange, "B");
    EXPECT_EQ(quote.condition, "A");
    EXPECT_EQ(quote.symbol, "AAPL250321C00200000");

    std::cout << "PASSED" << std::endl;
}

// ---------------------------------------------------------------------------
// OptionBar tests
// ---------------------------------------------------------------------------

TEST(OptionBarTest, ParseFromJson) {
    std::cout << "\n=== Test: OptionBarTest::ParseFromJson ===" << std::endl;

    json j = make_option_bar_json();
    OptionBar bar(j);

    EXPECT_EQ(bar.open, Decimal(5.50));
    EXPECT_EQ(bar.high, Decimal(6.00));
    EXPECT_EQ(bar.low, Decimal(5.30));
    EXPECT_EQ(bar.close, Decimal(5.75));
    EXPECT_EQ(bar.volume, Decimal(15000));
    EXPECT_EQ(bar.trade_count, 850u);
    EXPECT_EQ(bar.vwap, Decimal(5.65));
    EXPECT_EQ(bar.symbol, "AAPL250321C00200000");

    std::cout << "PASSED" << std::endl;
}

TEST(OptionBarTest, ParseMissingTradeCount) {
    json j = make_option_bar_json();
    j.erase("n");
    OptionBar bar(j);

    EXPECT_EQ(bar.trade_count, 0u);
    EXPECT_EQ(bar.close, Decimal(5.75));
}

// ---------------------------------------------------------------------------
// OptionSnapshot tests
// ---------------------------------------------------------------------------

TEST(OptionSnapshotTest, ParseFromJson) {
    std::cout << "\n=== Test: OptionSnapshotTest::ParseFromJson ===" << std::endl;

    json j = make_option_snapshot_json();
    OptionSnapshot snap(j, "AAPL250321C00200000");

    EXPECT_EQ(snap.symbol, "AAPL250321C00200000");
    EXPECT_EQ(snap.latest_trade.price, Decimal(5.75));
    EXPECT_EQ(snap.latest_quote.ask_price, Decimal(5.90));
    EXPECT_EQ(snap.implied_volatility, Decimal(0.35));
    EXPECT_EQ(snap.greeks_delta, Decimal(0.65));
    EXPECT_EQ(snap.greeks_gamma, Decimal(0.03));
    EXPECT_EQ(snap.greeks_theta, Decimal(-0.05));
    EXPECT_EQ(snap.greeks_vega, Decimal(0.12));
    EXPECT_EQ(snap.greeks_rho, Decimal(0.08));

    std::cout << "PASSED" << std::endl;
}

TEST(OptionSnapshotTest, ParseMissingGreeks) {
    json j = make_option_snapshot_json();
    j.erase("greeks");
    OptionSnapshot snap(j);

    EXPECT_EQ(snap.greeks_delta, Decimal());
    EXPECT_EQ(snap.greeks_gamma, Decimal());
    EXPECT_EQ(snap.implied_volatility, Decimal(0.35));
}

TEST(OptionSnapshotTest, ParseNullLatestTrade) {
    json j = make_option_snapshot_json();
    j["latestTrade"] = nullptr;
    OptionSnapshot snap(j);

    EXPECT_EQ(snap.latest_trade.price, Decimal());
    EXPECT_EQ(snap.latest_quote.ask_price, Decimal(5.90));
}

TEST(OptionSnapshotTest, ToStringIncludesAllFields) {
    OptionSnapshot snap(make_option_snapshot_json(), "AAPL250321C00200000");

    std::string str = snap.to_string();

    EXPECT_NE(str.find("Option Snapshot:"), std::string::npos);
    EXPECT_NE(str.find("Symbol: AAPL250321C00200000"), std::string::npos);
    EXPECT_NE(str.find("Latest Trade Price: 5.75"), std::string::npos);
    EXPECT_NE(str.find("Latest Trade Size: 10"), std::string::npos);
    EXPECT_NE(str.find("Latest Trade Exchange: C"), std::string::npos);
    EXPECT_NE(str.find("Latest Trade Condition: I"), std::string::npos);
    EXPECT_NE(str.find("Latest Quote Ask Price: 5.9"), std::string::npos);
    EXPECT_NE(str.find("Latest Quote Ask Size: 50"), std::string::npos);
    EXPECT_NE(str.find("Latest Quote Bid Price: 5.6"), std::string::npos);
    EXPECT_NE(str.find("Latest Quote Bid Size: 40"), std::string::npos);
    EXPECT_NE(str.find("Latest Quote Bid Exchange: B"), std::string::npos);
    EXPECT_NE(str.find("Latest Quote Condition: A"), std::string::npos);
    EXPECT_NE(str.find("Implied Volatility: 0.35"), std::string::npos);
    EXPECT_NE(str.find("Greeks Delta: 0.65"), std::string::npos);
    EXPECT_NE(str.find("Greeks Gamma: 0.03"), std::string::npos);
    EXPECT_NE(str.find("Greeks Theta: -0.05"), std::string::npos);
    EXPECT_NE(str.find("Greeks Vega: 0.12"), std::string::npos);
    EXPECT_NE(str.find("Greeks Rho: 0.08"), std::string::npos);
}

TEST(OptionSnapshotTest, ToStringCanOmitSymbol) {
    OptionSnapshot snap(make_option_snapshot_json(), "AAPL250321C00200000");

    std::string str = snap.to_string(false);

    EXPECT_NE(str.find("Option Snapshot:"), std::string::npos);
    EXPECT_EQ(str.find("Symbol: AAPL250321C00200000"), std::string::npos);
    EXPECT_NE(str.find("Latest Trade Price: 5.75"), std::string::npos);
    EXPECT_NE(str.find("Implied Volatility: 0.35"), std::string::npos);
}
