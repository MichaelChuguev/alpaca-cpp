#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/model/data/crypto.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_crypto_trade_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["p"] = "67234.50";
    j["s"] = "0.5";
    j["tks"] = "B";
    j["i"] = 99001;
    j["S"] = "BTC/USD";
    return j;
}

json make_crypto_quote_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["bp"] = "67230.00";
    j["bs"] = "1.2";
    j["ap"] = "67240.00";
    j["as"] = "0.8";
    j["S"] = "BTC/USD";
    return j;
}

json make_crypto_bar_json() {
    json j;
    j["t"] = "2026-03-10T14:00:00Z";
    j["o"] = "67100.00";
    j["h"] = "67500.00";
    j["l"] = "67050.00";
    j["c"] = "67234.50";
    j["v"] = "125.75";
    j["n"] = 4500;
    j["vw"] = "67200.00";
    j["S"] = "BTC/USD";
    return j;
}

json make_orderbook_entry_json(const std::string& price, const std::string& size) {
    json j;
    j["p"] = price;
    j["s"] = size;
    return j;
}

json make_crypto_orderbook_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["b"] = json::array();
    j["b"].push_back(make_orderbook_entry_json("67230.00", "1.5"));
    j["b"].push_back(make_orderbook_entry_json("67225.00", "2.0"));
    j["a"] = json::array();
    j["a"].push_back(make_orderbook_entry_json("67240.00", "0.8"));
    j["a"].push_back(make_orderbook_entry_json("67245.00", "1.2"));
    return j;
}

json make_crypto_snapshot_json() {
    json j;
    j["latestTrade"] = make_crypto_trade_json();
    j["latestQuote"] = make_crypto_quote_json();
    j["minuteBar"] = make_crypto_bar_json();
    j["dailyBar"] = make_crypto_bar_json();
    j["prevDailyBar"] = make_crypto_bar_json();
    return j;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// CryptoTrade tests
// ---------------------------------------------------------------------------

TEST(CryptoTradeTest, ParseFromJson) {
    std::cout << "\n=== Test: CryptoTradeTest::ParseFromJson ===" << std::endl;

    json j = make_crypto_trade_json();
    CryptoTrade trade(j);

    EXPECT_EQ(trade.price, Decimal(67234.50));
    EXPECT_EQ(trade.size, Decimal(0.5));
    EXPECT_EQ(trade.taker_side, "B");
    EXPECT_EQ(trade.id, 99001u);
    EXPECT_EQ(trade.symbol, "BTC/USD");

    std::cout << "PASSED" << std::endl;
}

TEST(CryptoTradeTest, ParseMissingId) {
    json j = make_crypto_trade_json();
    j.erase("i");
    CryptoTrade trade(j);

    EXPECT_EQ(trade.id, 0u);
    EXPECT_EQ(trade.price, Decimal(67234.50));
}

TEST(CryptoTradeTest, DefaultConstruct) {
    CryptoTrade trade;
    EXPECT_EQ(trade.price, Decimal());
    EXPECT_EQ(trade.id, 0u);
}

// ---------------------------------------------------------------------------
// CryptoQuote tests
// ---------------------------------------------------------------------------

TEST(CryptoQuoteTest, ParseFromJson) {
    std::cout << "\n=== Test: CryptoQuoteTest::ParseFromJson ===" << std::endl;

    json j = make_crypto_quote_json();
    CryptoQuote quote(j);

    EXPECT_EQ(quote.bid_price, Decimal(67230.00));
    EXPECT_EQ(quote.bid_size, Decimal(1.2));
    EXPECT_EQ(quote.ask_price, Decimal(67240.00));
    EXPECT_EQ(quote.ask_size, Decimal(0.8));
    EXPECT_EQ(quote.symbol, "BTC/USD");

    std::cout << "PASSED" << std::endl;
}

// ---------------------------------------------------------------------------
// CryptoBar tests
// ---------------------------------------------------------------------------

TEST(CryptoBarTest, ParseFromJson) {
    std::cout << "\n=== Test: CryptoBarTest::ParseFromJson ===" << std::endl;

    json j = make_crypto_bar_json();
    CryptoBar bar(j);

    EXPECT_EQ(bar.open, Decimal(67100.00));
    EXPECT_EQ(bar.high, Decimal(67500.00));
    EXPECT_EQ(bar.low, Decimal(67050.00));
    EXPECT_EQ(bar.close, Decimal(67234.50));
    EXPECT_EQ(bar.volume, Decimal(125.75));
    EXPECT_EQ(bar.trade_count, 4500u);
    EXPECT_EQ(bar.vwap, Decimal(67200.00));
    EXPECT_EQ(bar.symbol, "BTC/USD");

    std::cout << "PASSED" << std::endl;
}

TEST(CryptoBarTest, ParseMissingTradeCount) {
    json j = make_crypto_bar_json();
    j.erase("n");
    CryptoBar bar(j);

    EXPECT_EQ(bar.trade_count, 0u);
}

// ---------------------------------------------------------------------------
// OrderbookEntry tests
// ---------------------------------------------------------------------------

TEST(OrderbookEntryTest, ParseFromJson) {
    json j = make_orderbook_entry_json("67230.00", "1.5");
    OrderbookEntry entry(j);

    EXPECT_EQ(entry.price, Decimal(67230.00));
    EXPECT_EQ(entry.size, Decimal(1.5));
}

// ---------------------------------------------------------------------------
// CryptoOrderbook tests
// ---------------------------------------------------------------------------

TEST(CryptoOrderbookTest, ParseFromJson) {
    std::cout << "\n=== Test: CryptoOrderbookTest::ParseFromJson ===" << std::endl;

    json j = make_crypto_orderbook_json();
    CryptoOrderbook ob(j, "BTC/USD");

    EXPECT_EQ(ob.symbol, "BTC/USD");
    ASSERT_EQ(ob.bids.size(), 2u);
    EXPECT_EQ(ob.bids[0].price, Decimal(67230.00));
    EXPECT_EQ(ob.bids[0].size, Decimal(1.5));
    EXPECT_EQ(ob.bids[1].price, Decimal(67225.00));
    ASSERT_EQ(ob.asks.size(), 2u);
    EXPECT_EQ(ob.asks[0].price, Decimal(67240.00));
    EXPECT_EQ(ob.asks[1].size, Decimal(1.2));

    std::cout << "PASSED" << std::endl;
}

TEST(CryptoOrderbookTest, ParseEmptyOrderbook) {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    // No bids or asks
    CryptoOrderbook ob(j);

    EXPECT_TRUE(ob.bids.empty());
    EXPECT_TRUE(ob.asks.empty());
}

// ---------------------------------------------------------------------------
// CryptoSnapshot tests
// ---------------------------------------------------------------------------

TEST(CryptoSnapshotTest, ParseFromJson) {
    std::cout << "\n=== Test: CryptoSnapshotTest::ParseFromJson ===" << std::endl;

    json j = make_crypto_snapshot_json();
    CryptoSnapshot snap(j, "BTC/USD");

    EXPECT_EQ(snap.symbol, "BTC/USD");
    EXPECT_EQ(snap.latest_trade.price, Decimal(67234.50));
    EXPECT_EQ(snap.latest_quote.bid_price, Decimal(67230.00));
    EXPECT_EQ(snap.minute_bar.close, Decimal(67234.50));
    EXPECT_EQ(snap.daily_bar.high, Decimal(67500.00));
    EXPECT_EQ(snap.prev_daily_bar.low, Decimal(67050.00));

    std::cout << "PASSED" << std::endl;
}

TEST(CryptoSnapshotTest, ParsePartialSnapshot) {
    json j;
    j["latestTrade"] = make_crypto_trade_json();
    j["latestQuote"] = nullptr;
    // Missing bars

    CryptoSnapshot snap(j);
    EXPECT_EQ(snap.latest_trade.price, Decimal(67234.50));
    EXPECT_EQ(snap.latest_quote.bid_price, Decimal());
}
