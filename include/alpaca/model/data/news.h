#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// News Article
// ---------------------------------------------------------------------------

struct NewsImage {
    std::string url;
    std::string size;  // "thumb", "small", "large"

    NewsImage() = default;
    NewsImage(const json& j)
        : url(parse_string(j, "url")),
          size(parse_string(j, "size"))
    {}
};

struct NewsArticle {
    uint64_t    id{0};
    std::string headline;
    std::string author;
    DateTime    created_at;
    DateTime    updated_at;
    std::string summary;
    std::string content;
    std::string url;
    std::string source;
    std::vector<NewsImage>    images;
    std::vector<std::string>  symbols;

    NewsArticle() = default;
    NewsArticle(const json& j)
        : headline(parse_string(j, "headline")),
          author(parse_string(j, "author")),
          created_at(parse_datetime(j, "created_at")),
          updated_at(parse_datetime(j, "updated_at")),
          summary(parse_string(j, "summary")),
          content(parse_string(j, "content")),
          url(parse_string(j, "url")),
          source(parse_string(j, "source"))
    {
        if (j.contains("id") && !j["id"].is_null()) id = j["id"].get<uint64_t>();
        if (j.contains("images") && j["images"].is_array()) {
            for (const auto& img : j["images"]) images.emplace_back(img);
        }
        if (j.contains("symbols") && j["symbols"].is_array()) {
            symbols = j["symbols"].get<std::vector<std::string>>();
        }
    }
};

// ---------------------------------------------------------------------------
// Screener: Most Actives
// ---------------------------------------------------------------------------

struct MostActive {
    std::string symbol;
    Decimal     volume{};
    Decimal     trade_count{};

    MostActive() = default;
    MostActive(const json& j)
        : symbol(parse_string(j, "symbol")),
          volume(parse_decimal(j, "volume")),
          trade_count(parse_decimal(j, "trade_count"))
    {}
};

// ---------------------------------------------------------------------------
// Screener: Movers
// ---------------------------------------------------------------------------

struct Mover {
    std::string symbol;
    Decimal     price{};
    Decimal     change{};
    Decimal     percent_change{};
    Decimal     volume{};

    Mover() = default;
    Mover(const json& j)
        : symbol(parse_string(j, "symbol")),
          price(parse_decimal(j, "price")),
          change(parse_decimal(j, "change")),
          percent_change(parse_decimal(j, "percent_change")),
          volume(parse_decimal(j, "volume"))
    {}
};

// ---------------------------------------------------------------------------
// Corporate Actions
// ---------------------------------------------------------------------------

struct CorporateAction {
    std::string id;
    std::string corporate_actions_type;  // "dividend", "split", "merger", "spinoff", etc.
    std::string symbol;
    DateTime    date;
    std::string description;

    // Common fields (varies by type)
    Decimal     cash{};
    Decimal     old_rate{};
    Decimal     new_rate{};
    std::string process_date;
    std::string payable_date;
    std::string record_date;
    std::string ex_date;

    CorporateAction() = default;
    CorporateAction(const json& j)
        : id(parse_string(j, "id")),
          corporate_actions_type(parse_string(j, "type")),
          symbol(parse_string(j, "symbol")),
          date(parse_datetime(j, "date")),
          description(parse_string(j, "description")),
          cash(parse_decimal(j, "cash")),
          old_rate(parse_decimal(j, "old_rate")),
          new_rate(parse_decimal(j, "new_rate")),
          process_date(parse_string(j, "process_date")),
          payable_date(parse_string(j, "payable_date")),
          record_date(parse_string(j, "record_date")),
          ex_date(parse_string(j, "ex_date"))
    {}
};

// ---------------------------------------------------------------------------
// Forex Rate
// ---------------------------------------------------------------------------

struct ForexRate {
    DateTime    timestamp;
    Decimal     bid_price{};
    Decimal     ask_price{};
    Decimal     mid_price{};
    std::string currency_pair;

    ForexRate() = default;
    ForexRate(const json& j, const std::string& pair = "")
        : timestamp(parse_datetime(j, "t")),
          bid_price(parse_decimal(j, "bp")),
          ask_price(parse_decimal(j, "ap")),
          mid_price(parse_decimal(j, "mp")),
          currency_pair(pair)
    {}
};

// ---------------------------------------------------------------------------
// Fixed Income Price
// ---------------------------------------------------------------------------

struct FixedIncomePrice {
    DateTime    timestamp;
    Decimal     rate{};
    std::string symbol;

    FixedIncomePrice() = default;
    FixedIncomePrice(const json& j, const std::string& sym = "")
        : timestamp(parse_datetime(j, "t")),
          rate(parse_decimal(j, "r")),
          symbol(sym)
    {}
};

} // namespace alpaca
