#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/model/data/stock.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_stock_trade_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00.123456Z";
    j["p"] = "175.25";
    j["s"] = "100";
    j["x"] = "V";
    j["i"] = 12345;
    j["c"] = json::array({"@", "T"});
    j["z"] = "C";
    j["u"] = "";
    j["S"] = "AAPL";
    return j;
}

json make_stock_quote_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00.000000Z";
    j["ap"] = "175.30";
    j["as"] = "200";
    j["ax"] = "Q";
    j["bp"] = "175.20";
    j["bs"] = "300";
    j["bx"] = "P";
    j["c"] = json::array({"R"});
    j["z"] = "C";
    j["S"] = "AAPL";
    return j;
}

json make_stock_bar_json() {
    json j;
    j["t"] = "2026-03-10T00:00:00Z";
    j["o"] = "174.50";
    j["h"] = "176.00";
    j["l"] = "173.80";
    j["c"] = "175.25";
    j["v"] = "50000000";
    j["n"] = 350000;
    j["vw"] = "175.10";
    j["S"] = "AAPL";
    return j;
}

json make_stock_auction_json() {
    json j;
    j["d"] = "2026-03-10";
    j["S"] = "AAPL";
    j["o"] = json::array();
    json opening;
    opening["t"] = "2026-03-10T09:30:00Z";
    opening["p"] = "174.80";
    opening["s"] = "10000";
    opening["x"] = "Q";
    opening["c"] = "O";
    j["o"].push_back(opening);
    j["c"] = json::array();
    json closing;
    closing["t"] = "2026-03-10T16:00:00Z";
    closing["p"] = "175.50";
    closing["s"] = "15000";
    closing["x"] = "Q";
    closing["c"] = "6";
    j["c"].push_back(closing);
    return j;
}

json make_stock_snapshot_json() {
    json j;
    j["latestTrade"] = make_stock_trade_json();
    j["latestQuote"] = make_stock_quote_json();
    j["minuteBar"] = make_stock_bar_json();
    j["dailyBar"] = make_stock_bar_json();
    j["prevDailyBar"] = make_stock_bar_json();
    return j;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// StockTrade tests
// ---------------------------------------------------------------------------

TEST(StockTradeTest, ParseFromJson) {
    std::cout << "\n=== Test: StockTradeTest::ParseFromJson ===" << std::endl;

    json j = make_stock_trade_json();
    StockTrade trade(j);

    EXPECT_EQ(trade.price, Decimal(175.25));
    EXPECT_EQ(trade.size, Decimal(100));
    EXPECT_EQ(trade.exchange, "V");
    EXPECT_EQ(trade.id, 12345u);
    EXPECT_EQ(trade.tape, "C");
    EXPECT_EQ(trade.symbol, "AAPL");
    ASSERT_EQ(trade.conditions.size(), 2u);
    EXPECT_EQ(trade.conditions[0], "@");
    EXPECT_EQ(trade.conditions[1], "T");

    std::cout << "PASSED" << std::endl;
}

TEST(StockTradeTest, ParseMissingOptionalFields) {
    std::cout << "\n=== Test: StockTradeTest::ParseMissingOptionalFields ===" << std::endl;

    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["p"] = "175.25";
    j["s"] = "100";
    j["x"] = "V";
    // No id, conditions, tape, update, symbol

    StockTrade trade(j);

    EXPECT_EQ(trade.price, Decimal(175.25));
    EXPECT_EQ(trade.id, 0u);
    EXPECT_TRUE(trade.conditions.empty());
    EXPECT_TRUE(trade.tape.empty());
    EXPECT_TRUE(trade.symbol.empty());

    std::cout << "PASSED" << std::endl;
}

TEST(StockTradeTest, DefaultConstruct) {
    StockTrade trade;
    EXPECT_EQ(trade.price, Decimal());
    EXPECT_EQ(trade.id, 0u);
    EXPECT_TRUE(trade.conditions.empty());
}

// ---------------------------------------------------------------------------
// StockQuote tests
// ---------------------------------------------------------------------------

TEST(StockQuoteTest, ParseFromJson) {
    std::cout << "\n=== Test: StockQuoteTest::ParseFromJson ===" << std::endl;

    json j = make_stock_quote_json();
    StockQuote quote(j);

    EXPECT_EQ(quote.ask_price, Decimal(175.30));
    EXPECT_EQ(quote.ask_size, Decimal(200));
    EXPECT_EQ(quote.ask_exchange, "Q");
    EXPECT_EQ(quote.bid_price, Decimal(175.20));
    EXPECT_EQ(quote.bid_size, Decimal(300));
    EXPECT_EQ(quote.bid_exchange, "P");
    EXPECT_EQ(quote.tape, "C");
    EXPECT_EQ(quote.symbol, "AAPL");
    ASSERT_EQ(quote.conditions.size(), 1u);
    EXPECT_EQ(quote.conditions[0], "R");

    std::cout << "PASSED" << std::endl;
}

TEST(StockQuoteTest, ParseEmptyConditions) {
    json j = make_stock_quote_json();
    j["c"] = json::array();
    StockQuote quote(j);

    EXPECT_TRUE(quote.conditions.empty());
}

// ---------------------------------------------------------------------------
// StockBar tests
// ---------------------------------------------------------------------------

TEST(StockBarTest, ParseFromJson) {
    std::cout << "\n=== Test: StockBarTest::ParseFromJson ===" << std::endl;

    json j = make_stock_bar_json();
    StockBar bar(j);

    EXPECT_EQ(bar.open, Decimal(174.50));
    EXPECT_EQ(bar.high, Decimal(176.00));
    EXPECT_EQ(bar.low, Decimal(173.80));
    EXPECT_EQ(bar.close, Decimal(175.25));
    EXPECT_EQ(bar.volume, Decimal(50000000));
    EXPECT_EQ(bar.trade_count, 350000u);
    EXPECT_EQ(bar.vwap, Decimal(175.10));
    EXPECT_EQ(bar.symbol, "AAPL");

    std::cout << "PASSED" << std::endl;
}

TEST(StockBarTest, ParseMissingTradeCount) {
    json j = make_stock_bar_json();
    j.erase("n");
    StockBar bar(j);

    EXPECT_EQ(bar.trade_count, 0u);
    EXPECT_EQ(bar.close, Decimal(175.25));
}

TEST(StockBarTest, ToString) {
    StockBar bar(make_stock_bar_json());

    const std::string str = bar.to_string();

    EXPECT_NE(str.find("Stock Bar:"), std::string::npos);
    EXPECT_NE(str.find("Timestamp: 2026-03-10"), std::string::npos);
    EXPECT_NE(str.find("Open: 174.5"), std::string::npos);
    EXPECT_NE(str.find("High: 176"), std::string::npos);
    EXPECT_NE(str.find("Low: 173.8"), std::string::npos);
    EXPECT_NE(str.find("Close: 175.25"), std::string::npos);
    EXPECT_NE(str.find("Volume: 50000000"), std::string::npos);
    EXPECT_NE(str.find("Trade Count: 350000"), std::string::npos);
    EXPECT_NE(str.find("VWAP: 175.1"), std::string::npos);
    EXPECT_NE(str.find("Symbol: AAPL"), std::string::npos);
}

// ---------------------------------------------------------------------------
// StockAuction tests
// ---------------------------------------------------------------------------

TEST(StockAuctionTest, ParseFromJson) {
    std::cout << "\n=== Test: StockAuctionTest::ParseFromJson ===" << std::endl;

    json j = make_stock_auction_json();
    StockAuction auction(j);

    EXPECT_EQ(auction.date, "2026-03-10");
    EXPECT_EQ(auction.symbol, "AAPL");
    ASSERT_EQ(auction.opening.size(), 1u);
    EXPECT_EQ(auction.opening[0].price, Decimal(174.80));
    EXPECT_EQ(auction.opening[0].size, Decimal(10000));
    EXPECT_EQ(auction.opening[0].exchange, "Q");
    EXPECT_EQ(auction.opening[0].condition, "O");
    ASSERT_EQ(auction.closing.size(), 1u);
    EXPECT_EQ(auction.closing[0].price, Decimal(175.50));
    EXPECT_EQ(auction.closing[0].size, Decimal(15000));

    std::cout << "PASSED" << std::endl;
}

TEST(StockAuctionTest, ParseEmptyAuctions) {
    json j;
    j["d"] = "2026-03-10";
    j["S"] = "AAPL";
    // No opening or closing arrays
    StockAuction auction(j);

    EXPECT_TRUE(auction.opening.empty());
    EXPECT_TRUE(auction.closing.empty());
}

// ---------------------------------------------------------------------------
// StockSnapshot tests
// ---------------------------------------------------------------------------

TEST(StockSnapshotTest, ParseFromJson) {
    std::cout << "\n=== Test: StockSnapshotTest::ParseFromJson ===" << std::endl;

    json j = make_stock_snapshot_json();
    StockSnapshot snap(j, "AAPL");

    EXPECT_EQ(snap.symbol, "AAPL");
    EXPECT_EQ(snap.latest_trade.price, Decimal(175.25));
    EXPECT_EQ(snap.latest_quote.ask_price, Decimal(175.30));
    EXPECT_EQ(snap.minute_bar.close, Decimal(175.25));
    EXPECT_EQ(snap.daily_bar.high, Decimal(176.00));
    EXPECT_EQ(snap.prev_daily_bar.low, Decimal(173.80));

    std::cout << "PASSED" << std::endl;
}

TEST(StockSnapshotTest, ParsePartialSnapshot) {
    json j;
    j["latestTrade"] = make_stock_trade_json();
    j["latestQuote"] = nullptr;
    // Missing minuteBar, dailyBar, prevDailyBar

    StockSnapshot snap(j);
    EXPECT_EQ(snap.latest_trade.price, Decimal(175.25));
    EXPECT_EQ(snap.latest_quote.ask_price, Decimal());
}

// ---------------------------------------------------------------------------
// StockMetaCondition tests
// ---------------------------------------------------------------------------

TEST(StockMetaConditionTest, Construct) {
    StockMetaCondition cond("@", "Regular Sale");
    EXPECT_EQ(cond.code, "@");
    EXPECT_EQ(cond.name, "Regular Sale");
}
