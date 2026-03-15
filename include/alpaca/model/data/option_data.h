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
// Option Trade
// ---------------------------------------------------------------------------

struct OptionTrade {
    DateTime    timestamp;
    Decimal     price{};
    Decimal     size{};
    std::string exchange;
    std::string condition;
    std::string symbol;

    OptionTrade() = default;
    OptionTrade(const json& j)
        : timestamp(parse_datetime(j, "t")),
          price(parse_decimal(j, "p")),
          size(parse_decimal(j, "s")),
          exchange(parse_string(j, "x")),
          condition(parse_string(j, "c")),
          symbol(parse_string(j, "S"))
    {}
};

// ---------------------------------------------------------------------------
// Option Quote (used in latest quotes and snapshots)
// ---------------------------------------------------------------------------

struct OptionQuote {
    DateTime    timestamp;
    Decimal     ask_price{};
    Decimal     ask_size{};
    std::string ask_exchange;
    Decimal     bid_price{};
    Decimal     bid_size{};
    std::string bid_exchange;
    std::string condition;
    std::string symbol;

    OptionQuote() = default;
    OptionQuote(const json& j)
        : timestamp(parse_datetime(j, "t")),
          ask_price(parse_decimal(j, "ap")),
          ask_size(parse_decimal(j, "as")),
          ask_exchange(parse_string(j, "ax")),
          bid_price(parse_decimal(j, "bp")),
          bid_size(parse_decimal(j, "bs")),
          bid_exchange(parse_string(j, "bx")),
          condition(parse_string(j, "c")),
          symbol(parse_string(j, "S"))
    {}
};

// ---------------------------------------------------------------------------
// Option Bar
// ---------------------------------------------------------------------------

struct OptionBar {
    DateTime timestamp;
    Decimal  open{};
    Decimal  high{};
    Decimal  low{};
    Decimal  close{};
    Decimal  volume{};
    uint64_t trade_count{0};
    Decimal  vwap{};
    std::string symbol;

    OptionBar() = default;
    OptionBar(const json& j)
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
// Option Snapshot
// ---------------------------------------------------------------------------

struct OptionSnapshot {
    OptionTrade latest_trade;
    OptionQuote latest_quote;
    Decimal     implied_volatility{};
    Decimal     greeks_delta{};
    Decimal     greeks_gamma{};
    Decimal     greeks_theta{};
    Decimal     greeks_vega{};
    Decimal     greeks_rho{};
    std::string symbol;

    OptionSnapshot() = default;
    OptionSnapshot(const json& j, const std::string& sym = "")
        : symbol(sym)
    {
        if (j.contains("latestTrade") && !j["latestTrade"].is_null())
            latest_trade = OptionTrade(j["latestTrade"]);
        if (j.contains("latestQuote") && !j["latestQuote"].is_null())
            latest_quote = OptionQuote(j["latestQuote"]);
        implied_volatility = parse_decimal(j, "impliedVolatility");
        if (j.contains("greeks") && !j["greeks"].is_null()) {
            const auto& g = j["greeks"];
            greeks_delta = parse_decimal(g, "delta");
            greeks_gamma = parse_decimal(g, "gamma");
            greeks_theta = parse_decimal(g, "theta");
            greeks_vega  = parse_decimal(g, "vega");
            greeks_rho   = parse_decimal(g, "rho");
        }
    }
};

} // namespace alpaca
