#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Models for the real-time market-data WebSocket stream
// (wss://stream.data.alpaca.markets/v2/...)
//
// Messages arrive as JSON arrays: [{"T":"t", ...}, {"T":"q", ...}, ...]
// ---------------------------------------------------------------------------

/// A single real-time trade.
struct StreamTrade {
    std::string symbol;         // "S"
    Decimal     price{};     // "p"
    Decimal     size{};      // "s"
    DateTime    timestamp;      // "t"
    std::string exchange;       // "x"
    uint64_t    id{0};          // "i"
    std::string tape;           // "z"

    static StreamTrade from_json(const nlohmann::json& j) {
        StreamTrade t;
        t.symbol    = parse_string(j, "S");
        t.price     = parse_decimal(j, "p");
        t.size      = parse_decimal(j, "s");
        t.timestamp = parse_datetime(j, "t");
        t.exchange  = parse_string(j, "x");
        if (j.contains("i") && !j["i"].is_null()) t.id = j["i"].get<uint64_t>();
        t.tape      = parse_string(j, "z");
        return t;
    }
};

/// A single real-time quote (NBBO update).
struct StreamQuote {
    std::string symbol;           // "S"
    Decimal     bid_price{};   // "bp"
    Decimal     bid_size{};    // "bs"
    std::string bid_exchange;     // "bx"
    Decimal     ask_price{};   // "ap"
    Decimal     ask_size{};    // "as"
    std::string ask_exchange;     // "ax"
    DateTime    timestamp;        // "t"
    std::string tape;             // "z"
    std::vector<std::string> conditions; // "c"

    static StreamQuote from_json(const nlohmann::json& j) {
        StreamQuote q;
        q.symbol       = parse_string(j, "S");
        q.bid_price    = parse_decimal(j, "bp");
        q.bid_size     = parse_decimal(j, "bs");
        q.bid_exchange = parse_string(j, "bx");
        q.ask_price    = parse_decimal(j, "ap");
        q.ask_size     = parse_decimal(j, "as");
        q.ask_exchange = parse_string(j, "ax");
        q.timestamp    = parse_datetime(j, "t");
        q.tape         = parse_string(j, "z");
        if (j.contains("c") && j["c"].is_array()) {
            for (auto& c : j["c"]) q.conditions.push_back(c.get<std::string>());
        }
        return q;
    }
};

/// A single aggregated bar.
struct StreamBar {
    std::string symbol;         // "S"
    Decimal     open{};      // "o"
    Decimal     high{};      // "h"
    Decimal     low{};       // "l"
    Decimal     close{};     // "c"
    Decimal     volume{};    // "v"
    DateTime    timestamp;      // "t"
    uint64_t    trade_count{0}; // "n"
    Decimal     vwap{};      // "vw"

    static StreamBar from_json(const nlohmann::json& j) {
        StreamBar b;
        b.symbol      = parse_string(j, "S");
        b.open        = parse_decimal(j, "o");
        b.high        = parse_decimal(j, "h");
        b.low         = parse_decimal(j, "l");
        b.close       = parse_decimal(j, "c");
        b.volume      = parse_decimal(j, "v");
        b.timestamp   = parse_datetime(j, "t");
        if (j.contains("n") && !j["n"].is_null()) b.trade_count = j["n"].get<uint64_t>();
        b.vwap        = parse_decimal(j, "vw");
        return b;
    }
};

} // namespace alpaca
