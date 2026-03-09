#pragma once

#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Crypto Trade
// ---------------------------------------------------------------------------

struct CryptoTrade {
    DateTime    timestamp;
    Decimal     price{};
    Decimal     size{};
    std::string taker_side;  // "B" (buy) or "S" (sell)
    uint64_t    id{0};
    std::string symbol;

    CryptoTrade() = default;
    CryptoTrade(const json& j)
        : timestamp(parse_datetime(j, "t")),
          price(parse_decimal(j, "p")),
          size(parse_decimal(j, "s")),
          taker_side(parse_string(j, "tks")),
          symbol(parse_string(j, "S"))
    {
        if (j.contains("i") && !j["i"].is_null()) id = j["i"].get<uint64_t>();
    }
};

// ---------------------------------------------------------------------------
// Crypto Quote
// ---------------------------------------------------------------------------

struct CryptoQuote {
    DateTime timestamp;
    Decimal  bid_price{};
    Decimal  bid_size{};
    Decimal  ask_price{};
    Decimal  ask_size{};
    std::string symbol;

    CryptoQuote() = default;
    CryptoQuote(const json& j)
        : timestamp(parse_datetime(j, "t")),
          bid_price(parse_decimal(j, "bp")),
          bid_size(parse_decimal(j, "bs")),
          ask_price(parse_decimal(j, "ap")),
          ask_size(parse_decimal(j, "as")),
          symbol(parse_string(j, "S"))
    {}
};

// ---------------------------------------------------------------------------
// Crypto Bar
// ---------------------------------------------------------------------------

struct CryptoBar {
    DateTime timestamp;
    Decimal  open{};
    Decimal  high{};
    Decimal  low{};
    Decimal  close{};
    Decimal  volume{};
    uint64_t trade_count{0};
    Decimal  vwap{};
    std::string symbol;

    CryptoBar() = default;
    CryptoBar(const json& j)
        : timestamp(parse_datetime(j, "t")),
          open(parse_decimal(j, "o")),
          high(parse_decimal(j, "h")),
          low(parse_decimal(j, "l")),
          close(parse_decimal(j, "c")),
          volume(parse_decimal(j, "v")),
          vwap(parse_decimal(j, "vw")),
          symbol(parse_string(j, "S"))
    {
        if (j.contains("n") && !j["n"].is_null()) trade_count = j["n"].get<uint64_t>();
    }
};

// ---------------------------------------------------------------------------
// Crypto Orderbook Entry & Orderbook
// ---------------------------------------------------------------------------

struct OrderbookEntry {
    Decimal price{};
    Decimal size{};

    OrderbookEntry() = default;
    OrderbookEntry(const json& j)
        : price(parse_decimal(j, "p")),
          size(parse_decimal(j, "s"))
    {}
};

struct CryptoOrderbook {
    DateTime timestamp;
    std::vector<OrderbookEntry> bids;
    std::vector<OrderbookEntry> asks;
    std::string symbol;

    CryptoOrderbook() = default;
    CryptoOrderbook(const json& j, const std::string& sym = "")
        : timestamp(parse_datetime(j, "t")),
          symbol(sym)
    {
        if (j.contains("b") && j["b"].is_array()) {
            for (const auto& item : j["b"]) bids.emplace_back(item);
        }
        if (j.contains("a") && j["a"].is_array()) {
            for (const auto& item : j["a"]) asks.emplace_back(item);
        }
    }
};

// ---------------------------------------------------------------------------
// Crypto Snapshot
// ---------------------------------------------------------------------------

struct CryptoSnapshot {
    CryptoTrade latest_trade;
    CryptoQuote latest_quote;
    CryptoBar   minute_bar;
    CryptoBar   daily_bar;
    CryptoBar   prev_daily_bar;
    std::string symbol;

    CryptoSnapshot() = default;
    CryptoSnapshot(const json& j, const std::string& sym = "")
        : symbol(sym)
    {
        if (j.contains("latestTrade") && !j["latestTrade"].is_null())
            latest_trade = CryptoTrade(j["latestTrade"]);
        if (j.contains("latestQuote") && !j["latestQuote"].is_null())
            latest_quote = CryptoQuote(j["latestQuote"]);
        if (j.contains("minuteBar") && !j["minuteBar"].is_null())
            minute_bar = CryptoBar(j["minuteBar"]);
        if (j.contains("dailyBar") && !j["dailyBar"].is_null())
            daily_bar = CryptoBar(j["dailyBar"]);
        if (j.contains("prevDailyBar") && !j["prevDailyBar"].is_null())
            prev_daily_bar = CryptoBar(j["prevDailyBar"]);
    }
};

} // namespace alpaca
