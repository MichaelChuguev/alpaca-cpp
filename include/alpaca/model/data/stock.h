#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Stock Trade
// ---------------------------------------------------------------------------

struct StockTrade {
    DateTime    timestamp;
    Decimal     price{};
    Decimal     size{};
    std::string exchange;
    uint64_t    id{0};
    std::vector<std::string> conditions;
    std::string tape;
    std::string update;     // "update" field (for corrections)
    std::string symbol;     // populated when using multi-symbol endpoints

    StockTrade() = default;
    StockTrade(const json& j)
        : timestamp(parse_datetime(j, "t")),
          price(parse_decimal(j, "p")),
          size(parse_decimal(j, "s")),
          exchange(parse_string(j, "x")),
          tape(parse_string(j, "z")),
          update(parse_string(j, "u")),
          symbol(parse_string(j, "S"))
    {
        if (j.contains("i") && !j["i"].is_null()) id = j["i"].get<uint64_t>();
        if (j.contains("c") && j["c"].is_array()) {
            for (const auto& c : j["c"]) conditions.push_back(c.get<std::string>());
        }
    }
};

// ---------------------------------------------------------------------------
// Stock Quote
// ---------------------------------------------------------------------------

struct StockQuote {
    DateTime    timestamp;
    Decimal     ask_price{};
    Decimal     ask_size{};
    std::string ask_exchange;
    Decimal     bid_price{};
    Decimal     bid_size{};
    std::string bid_exchange;
    std::vector<std::string> conditions;
    std::string tape;
    std::string symbol;

    StockQuote() = default;
    StockQuote(const json& j)
        : timestamp(parse_datetime(j, "t")),
          ask_price(parse_decimal(j, "ap")),
          ask_size(parse_decimal(j, "as")),
          ask_exchange(parse_string(j, "ax")),
          bid_price(parse_decimal(j, "bp")),
          bid_size(parse_decimal(j, "bs")),
          bid_exchange(parse_string(j, "bx")),
          tape(parse_string(j, "z")),
          symbol(parse_string(j, "S"))
    {
        if (j.contains("c") && j["c"].is_array()) {
            for (const auto& c : j["c"]) conditions.push_back(c.get<std::string>());
        }
    }
};

// ---------------------------------------------------------------------------
// Stock Bar
// ---------------------------------------------------------------------------

struct StockBar {
    DateTime timestamp;
    Decimal  open{};
    Decimal  high{};
    Decimal  low{};
    Decimal  close{};
    Decimal  volume{};
    uint64_t trade_count{0};
    Decimal  vwap{};
    std::string symbol;

    StockBar() = default;
    StockBar(const json& j, const std::string& sym = "")
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
        if (symbol.empty()) symbol = sym;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Stock Bar:\n"
            << "Timestamp: " << timestamp.to_string() << "\n"
            << "Open: " << open << "\n"
            << "High: " << high << "\n"
            << "Low: " << low << "\n"
            << "Close: " << close << "\n"
            << "Volume: " << volume << "\n"
            << "Trade Count: " << trade_count << "\n"
            << "VWAP: " << vwap << "\n"
            << "Symbol: " << symbol << "\n";
        return oss.str();
    }
};

// ---------------------------------------------------------------------------
// Stock Auction
// ---------------------------------------------------------------------------

struct AuctionPrice {
    DateTime timestamp;
    Decimal  price{};
    Decimal  size{};
    std::string exchange;
    std::string condition;

    AuctionPrice() = default;
    AuctionPrice(const json& j)
        : timestamp(parse_datetime(j, "t")),
          price(parse_decimal(j, "p")),
          size(parse_decimal(j, "s")),
          exchange(parse_string(j, "x")),
          condition(parse_string(j, "c"))
    {}
};

struct StockAuction {
    std::string date;
    std::vector<AuctionPrice> opening;
    std::vector<AuctionPrice> closing;
    std::string symbol;

    StockAuction() = default;
    StockAuction(const json& j)
        : date(parse_string(j, "d")),
          symbol(parse_string(j, "S"))
    {
        if (j.contains("o") && j["o"].is_array()) {
            for (const auto& item : j["o"]) opening.emplace_back(item);
        }
        if (j.contains("c") && j["c"].is_array()) {
            for (const auto& item : j["c"]) closing.emplace_back(item);
        }
    }
};

// ---------------------------------------------------------------------------
// Stock Snapshot
// ---------------------------------------------------------------------------

struct StockSnapshot {
    StockTrade  latest_trade;
    StockQuote  latest_quote;
    StockBar    minute_bar;
    StockBar    daily_bar;
    StockBar    prev_daily_bar;
    std::string symbol;

    StockSnapshot() = default;
    StockSnapshot(const json& j, const std::string& sym = "")
        : symbol(sym)
    {
        if (j.contains("latestTrade") && !j["latestTrade"].is_null())
            latest_trade = StockTrade(j["latestTrade"]);
        if (j.contains("latestQuote") && !j["latestQuote"].is_null())
            latest_quote = StockQuote(j["latestQuote"]);
        if (j.contains("minuteBar") && !j["minuteBar"].is_null())
            minute_bar = StockBar(j["minuteBar"], sym);
        if (j.contains("dailyBar") && !j["dailyBar"].is_null())
            daily_bar = StockBar(j["dailyBar"], sym);
        if (j.contains("prevDailyBar") && !j["prevDailyBar"].is_null())
            prev_daily_bar = StockBar(j["prevDailyBar"], sym);
    }
};

// ---------------------------------------------------------------------------
// Stock Meta (Conditions & Exchanges)
// ---------------------------------------------------------------------------

struct StockMetaCondition {
    std::string code;
    std::string name;

    StockMetaCondition() = default;
    StockMetaCondition(const std::string& c, const std::string& n) : code(c), name(n) {}
};

} // namespace alpaca
