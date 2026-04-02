#include "alpaca/AlpacaMarketDataAPI.h"
#include "alpaca/core/error.h"
#include "alpaca/core/util.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Helper: convert vector of JSON objects into a typed vector
// ---------------------------------------------------------------------------
template <typename T>
static std::vector<T> parse_array(const json& j) {
    std::vector<T> out;
    out.reserve(j.size());
    for (const auto& item : j) {
        out.emplace_back(item);
    }
    return out;
}

static std::string append_page_token(std::string query, const std::string& page_token) {
    if (page_token.empty()) return query;

    query += (query.find('?') == std::string::npos ? "?" : "&");
    query += "page_token=" + url_encode(page_token);
    return query;
}

static std::string extract_next_page_token(const json& j) {
    if (j.contains("next_page_token") && !j["next_page_token"].is_null()) {
        return j["next_page_token"].get<std::string>();
    }
    return "";
}

template <typename Result, typename FetchFn, typename MergeFn>
static Result collect_paginated(
    const std::string& base_query,
    const std::string& page_token,
    FetchFn&& fetch_page,
    MergeFn&& merge_page) {

    Result result;
    std::string next_page_token = page_token;

    do {
        json j = fetch_page(append_page_token(base_query, next_page_token));
        merge_page(j, result);
        next_page_token = extract_next_page_token(j);
    } while (!next_page_token.empty());

    return result;
}

template <typename T>
static void merge_symbol_array_page(
    const json& j,
    const char* key,
    std::map<std::string, std::vector<T>>& result) {

    if (!j.contains(key) || !j[key].is_object()) {
        return;
    }

    for (const auto& [sym, items_json] : j[key].items()) {
        auto& items = result[sym];
        items.reserve(items.size() + items_json.size());
        for (const auto& item : items_json) {
            items.emplace_back(item);
        }
    }
}

template <typename T, typename Factory>
static void merge_symbol_array_page(
    const json& j,
    const char* key,
    std::map<std::string, std::vector<T>>& result,
    Factory&& factory) {

    if (!j.contains(key) || !j[key].is_object()) {
        return;
    }

    for (const auto& [sym, items_json] : j[key].items()) {
        auto& items = result[sym];
        items.reserve(items.size() + items_json.size());
        for (const auto& item : items_json) {
            items.push_back(factory(item, sym));
        }
    }
}

template <typename T>
static void merge_array_page(
    const json& j,
    const char* key,
    std::vector<T>& result) {

    if (!j.contains(key) || !j[key].is_array()) {
        return;
    }

    const auto& items_json = j[key];
    result.reserve(result.size() + items_json.size());
    for (const auto& item : items_json) {
        result.emplace_back(item);
    }
}

template <typename T, typename Factory>
static void merge_array_page(
    const json& j,
    const char* key,
    std::vector<T>& result,
    Factory&& factory) {

    if (!j.contains(key) || !j[key].is_array()) {
        return;
    }

    const auto& items_json = j[key];
    result.reserve(result.size() + items_json.size());
    for (const auto& item : items_json) {
        result.push_back(factory(item));
    }
}

template <typename T, typename Factory>
static void merge_symbol_object_page(
    const json& j,
    const char* key,
    std::map<std::string, T>& result,
    Factory&& factory) {

    if (!j.contains(key) || !j[key].is_object()) {
        return;
    }

    for (const auto& [sym, item_json] : j[key].items()) {
        result.insert_or_assign(sym, factory(item_json, sym));
    }
}

static void merge_array_field_page(
    const json& j,
    const char* key,
    json& result) {

    if (!result.is_object()) {
        result = json::object();
    }
    if (!result.contains(key) || !result[key].is_array()) {
        result[key] = json::array();
    }

    if (j.contains(key) && j[key].is_array()) {
        for (const auto& item : j[key]) {
            result[key].push_back(item);
        }
    }
}

static void merge_object_field_page(
    const json& j,
    const char* key,
    json& result) {

    if (!result.is_object()) {
        result = json::object();
    }
    if (!result.contains(key) || !result[key].is_object()) {
        result[key] = json::object();
    }

    if (j.contains(key) && j[key].is_object()) {
        for (const auto& [sym, item_json] : j[key].items()) {
            result[key][sym] = item_json;
        }
    }
}

static void merge_option_chain_page(
    const json& j,
    std::map<std::string, OptionSnapshot>& result) {

    merge_symbol_object_page<OptionSnapshot>(
        j,
        "snapshots",
        result,
        [](const json& snapshot_json, const std::string& sym) {
            return OptionSnapshot(snapshot_json, sym);
        });
}

static void merge_option_chain_page(
    const json& j,
    json& snapshots) {

    merge_object_field_page(j, "snapshots", snapshots);
}

// =====================================================================
//  STOCK ENDPOINTS
// =====================================================================

// ── Trades ───────────────────────────────────────────────────────────

std::map<std::string, std::vector<StockTrade>> AlpacaMarketDataAPI::get_stock_trades(
    const std::vector<std::string>& symbols,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v2/stocks/trades")
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<StockTrade>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<StockTrade>(j, "trades", result);
        });
}

std::vector<StockTrade> AlpacaMarketDataAPI::get_stock_trades_single(
    const std::string& symbol,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/trades")
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::vector<StockTrade>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_array_page<StockTrade>(j, "trades", result);
        });
}

std::map<std::string, StockTrade> AlpacaMarketDataAPI::get_stock_latest_trades(
    const std::vector<std::string>& symbols,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/trades/latest")
        .add_list("symbols", symbols)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = httpClient.get(qb.build());

    std::map<std::string, StockTrade> result;
    if (j.contains("trades") && j["trades"].is_object()) {
        for (auto& [sym, trade_json] : j["trades"].items()) {
            result.emplace(sym, StockTrade(trade_json));
        }
    }
    return result;
}

StockTrade AlpacaMarketDataAPI::get_stock_latest_trade_single(
    const std::string& symbol,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/trades/latest")
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = httpClient.get(qb.build());
    if (j.contains("trade") && !j["trade"].is_null()) {
        return StockTrade(j["trade"]);
    }
    return StockTrade();
}

// ── Quotes ───────────────────────────────────────────────────────────

std::map<std::string, std::vector<StockQuote>> AlpacaMarketDataAPI::get_stock_quotes(
    const std::vector<std::string>& symbols,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v2/stocks/quotes")
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<StockQuote>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<StockQuote>(j, "quotes", result);
        });
}

std::vector<StockQuote> AlpacaMarketDataAPI::get_stock_quotes_single(
    const std::string& symbol,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/quotes")
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::vector<StockQuote>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_array_page<StockQuote>(j, "quotes", result);
        });
}

std::map<std::string, StockQuote> AlpacaMarketDataAPI::get_stock_latest_quotes(
    const std::vector<std::string>& symbols,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/quotes/latest")
        .add_list("symbols", symbols)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = httpClient.get(qb.build());

    std::map<std::string, StockQuote> result;
    if (j.contains("quotes") && j["quotes"].is_object()) {
        for (auto& [sym, quote_json] : j["quotes"].items()) {
            result.emplace(sym, StockQuote(quote_json));
        }
    }
    return result;
}

StockQuote AlpacaMarketDataAPI::get_stock_latest_quote_single(
    const std::string& symbol,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/quotes/latest")
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = httpClient.get(qb.build());
    if (j.contains("quote") && !j["quote"].is_null()) {
        return StockQuote(j["quote"]);
    }
    return StockQuote();
}

// ── Bars ─────────────────────────────────────────────────────────────

std::map<std::string, std::vector<StockBar>> AlpacaMarketDataAPI::get_stock_bars(
    const std::vector<std::string>& symbols,
    DataTimeframe timeframe,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort,
    const std::string& adjustment) {

    auto qb = QueryBuilder("/v2/stocks/bars")
        .add_list("symbols", symbols)
        .add("timeframe", data_timeframe_to_string(timeframe))
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort))
        .add("adjustment", adjustment);

    return collect_paginated<std::map<std::string, std::vector<StockBar>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<StockBar>(
                j,
                "bars",
                result,
                [](const json& item, const std::string& sym) {
                    return StockBar(item, sym);
                });
        });
}

std::vector<StockBar> AlpacaMarketDataAPI::get_stock_bars_single(
    const std::string& symbol,
    DataTimeframe timeframe,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort,
    const std::string& adjustment) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/bars")
        .add("timeframe", data_timeframe_to_string(timeframe))
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort))
        .add("adjustment", adjustment);

    return collect_paginated<std::vector<StockBar>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [&symbol](const json& j, auto& result) {
            merge_array_page<StockBar>(
                j,
                "bars",
                result,
                [&symbol](const json& item) {
                    return StockBar(item, symbol);
                });
        });
}

std::map<std::string, StockBar> AlpacaMarketDataAPI::get_stock_latest_bars(
    const std::vector<std::string>& symbols,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/bars/latest")
        .add_list("symbols", symbols)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = get_json(qb.build());

    std::map<std::string, StockBar> result;
    if (j.contains("bars") && j["bars"].is_object()) {
        for (auto& [sym, bar_json] : j["bars"].items()) {
            result.emplace(sym, StockBar(bar_json, sym));
        }
    }
    return result;
}

StockBar AlpacaMarketDataAPI::get_stock_latest_bar_single(
    const std::string& symbol,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/bars/latest")
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = get_json(qb.build());
    if (j.contains("bar") && !j["bar"].is_null()) {
        return StockBar(j["bar"], symbol);
    }
    return StockBar();
}

// ── Auctions ─────────────────────────────────────────────────────────

std::map<std::string, std::vector<StockAuction>> AlpacaMarketDataAPI::get_stock_auctions(
    const std::vector<std::string>& symbols,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v2/stocks/auctions")
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<StockAuction>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<StockAuction>(j, "auctions", result);
        });
}

std::vector<StockAuction> AlpacaMarketDataAPI::get_stock_auctions_single(
    const std::string& symbol,
    const std::string& start, const std::string& end,
    DataFeed feed, const std::string& currency,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/auctions")
        .add("start", start)
        .add("end", end)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::vector<StockAuction>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_array_page<StockAuction>(j, "auctions", result);
        });
}

// ── Snapshots ────────────────────────────────────────────────────────

std::map<std::string, StockSnapshot> AlpacaMarketDataAPI::get_stock_snapshots(
    const std::vector<std::string>& symbols,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/snapshots")
        .add_list("symbols", symbols)
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = httpClient.get(qb.build());

    std::map<std::string, StockSnapshot> result;
    for (auto& [sym, snapshot_json] : j.items()) {
        result.emplace(sym, StockSnapshot(snapshot_json, sym));
    }
    return result;
}

StockSnapshot AlpacaMarketDataAPI::get_stock_snapshot_single(
    const std::string& symbol,
    DataFeed feed, const std::string& currency) {

    auto qb = QueryBuilder("/v2/stocks/" + url_encode(symbol) + "/snapshot")
        .add("feed", data_feed_to_string(resolve_feed(feed)))
        .add("currency", currency);

    json j = httpClient.get(qb.build());
    return StockSnapshot(j, symbol);
}

// ── Meta ─────────────────────────────────────────────────────────────

std::map<std::string, std::string> AlpacaMarketDataAPI::get_stock_meta_conditions(TickType tick_type) {
    json j = httpClient.get("/v2/stocks/meta/conditions/" + tick_type_to_string(tick_type));

    std::map<std::string, std::string> result;
    for (auto& [code, name] : j.items()) {
        result[code] = name.get<std::string>();
    }
    return result;
}

std::map<std::string, std::string> AlpacaMarketDataAPI::get_stock_meta_exchanges() {
    json j = httpClient.get("/v2/stocks/meta/exchanges");

    std::map<std::string, std::string> result;
    for (auto& [code, name] : j.items()) {
        result[code] = name.get<std::string>();
    }
    return result;
}

// =====================================================================
//  OPTION ENDPOINTS
// =====================================================================

std::map<std::string, std::vector<OptionBar>> AlpacaMarketDataAPI::get_option_bars(
    const std::vector<std::string>& symbols,
    DataTimeframe timeframe,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v1beta1/options/bars")
        .add_list("symbols", symbols)
        .add("timeframe", data_timeframe_to_string(timeframe))
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<OptionBar>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<OptionBar>(j, "bars", result);
        });
}

std::map<std::string, std::vector<OptionTrade>> AlpacaMarketDataAPI::get_option_trades(
    const std::vector<std::string>& symbols,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v1beta1/options/trades")
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<OptionTrade>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<OptionTrade>(j, "trades", result);
        });
}

std::map<std::string, OptionTrade> AlpacaMarketDataAPI::get_option_latest_trades(
    const std::vector<std::string>& symbols,
    OptionFeed feed) {

    auto qb = QueryBuilder("/v1beta1/options/trades/latest")
        .add_list("symbols", symbols)
        .add("feed", option_feed_to_string(resolve_option_feed(feed)));

    json j = httpClient.get(qb.build());

    std::map<std::string, OptionTrade> result;
    if (j.contains("trades") && j["trades"].is_object()) {
        for (auto& [sym, trade_json] : j["trades"].items()) {
            result.emplace(sym, OptionTrade(trade_json));
        }
    }
    return result;
}

std::map<std::string, OptionQuote> AlpacaMarketDataAPI::get_option_latest_quotes(
    const std::vector<std::string>& symbols,
    OptionFeed feed) {

    auto qb = QueryBuilder("/v1beta1/options/quotes/latest")
        .add_list("symbols", symbols)
        .add("feed", option_feed_to_string(resolve_option_feed(feed)));

    json j = httpClient.get(qb.build());

    std::map<std::string, OptionQuote> result;
    if (j.contains("quotes") && j["quotes"].is_object()) {
        for (auto& [sym, quote_json] : j["quotes"].items()) {
            result.emplace(sym, OptionQuote(quote_json));
        }
    }
    return result;
}

std::map<std::string, OptionSnapshot> AlpacaMarketDataAPI::get_option_snapshots(
    const std::vector<std::string>& symbols,
    OptionFeed feed,
    int limit, const std::string& page_token) {

    auto qb = QueryBuilder("/v1beta1/options/snapshots")
        .add_list("symbols", symbols)
        .add("feed", option_feed_to_string(resolve_option_feed(feed)))
        .add("limit", limit);

    return collect_paginated<std::map<std::string, OptionSnapshot>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_object_page<OptionSnapshot>(
                j,
                "snapshots",
                result,
                [](const json& snapshot_json, const std::string& sym) {
                    return OptionSnapshot(snapshot_json, sym);
                });
        });
}

std::map<std::string, OptionSnapshot> AlpacaMarketDataAPI::get_option_chain(
    const std::string& underlying_symbol,
    OptionFeed feed,
    const std::string& page_token,
    int limit) {

    auto qb = QueryBuilder("/v1beta1/options/snapshots/" + url_encode(underlying_symbol))
        .add("feed", option_feed_to_string(resolve_option_feed(feed)))
        .add("limit", limit);

    return collect_paginated<std::map<std::string, OptionSnapshot>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_option_chain_page(j, result);
        });
}

std::map<std::string, std::string> AlpacaMarketDataAPI::get_option_meta_conditions(TickType tick_type) {
    json j = httpClient.get("/v1beta1/options/meta/conditions/" + tick_type_to_string(tick_type));

    std::map<std::string, std::string> result;
    for (auto& [code, name] : j.items()) {
        result[code] = name.get<std::string>();
    }
    return result;
}

std::map<std::string, std::string> AlpacaMarketDataAPI::get_option_meta_exchanges() {
    json j = httpClient.get("/v1beta1/options/meta/exchanges");

    std::map<std::string, std::string> result;
    for (auto& [code, name] : j.items()) {
        result[code] = name.get<std::string>();
    }
    return result;
}

// =====================================================================
//  CRYPTO ENDPOINTS
// =====================================================================

std::map<std::string, std::vector<CryptoBar>> AlpacaMarketDataAPI::get_crypto_bars(
    const std::vector<std::string>& symbols,
    CryptoLoc loc, DataTimeframe timeframe,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/bars";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols)
        .add("timeframe", data_timeframe_to_string(timeframe))
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<CryptoBar>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<CryptoBar>(j, "bars", result);
        });
}

std::map<std::string, CryptoBar> AlpacaMarketDataAPI::get_crypto_latest_bars(
    const std::vector<std::string>& symbols, CryptoLoc loc) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/latest/bars";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols);

    json j = httpClient.get(qb.build());

    std::map<std::string, CryptoBar> result;
    if (j.contains("bars") && j["bars"].is_object()) {
        for (auto& [sym, bar_json] : j["bars"].items()) {
            result.emplace(sym, CryptoBar(bar_json));
        }
    }
    return result;
}

std::map<std::string, std::vector<CryptoTrade>> AlpacaMarketDataAPI::get_crypto_trades(
    const std::vector<std::string>& symbols,
    CryptoLoc loc,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/trades";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<CryptoTrade>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<CryptoTrade>(j, "trades", result);
        });
}

std::map<std::string, CryptoTrade> AlpacaMarketDataAPI::get_crypto_latest_trades(
    const std::vector<std::string>& symbols, CryptoLoc loc) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/latest/trades";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols);

    json j = httpClient.get(qb.build());

    std::map<std::string, CryptoTrade> result;
    if (j.contains("trades") && j["trades"].is_object()) {
        for (auto& [sym, trade_json] : j["trades"].items()) {
            result.emplace(sym, CryptoTrade(trade_json));
        }
    }
    return result;
}

std::map<std::string, std::vector<CryptoQuote>> AlpacaMarketDataAPI::get_crypto_quotes(
    const std::vector<std::string>& symbols,
    CryptoLoc loc,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/quotes";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<CryptoQuote>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_symbol_array_page<CryptoQuote>(j, "quotes", result);
        });
}

std::map<std::string, CryptoQuote> AlpacaMarketDataAPI::get_crypto_latest_quotes(
    const std::vector<std::string>& symbols, CryptoLoc loc) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/latest/quotes";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols);

    json j = httpClient.get(qb.build());

    std::map<std::string, CryptoQuote> result;
    if (j.contains("quotes") && j["quotes"].is_object()) {
        for (auto& [sym, quote_json] : j["quotes"].items()) {
            result.emplace(sym, CryptoQuote(quote_json));
        }
    }
    return result;
}

std::map<std::string, CryptoOrderbook> AlpacaMarketDataAPI::get_crypto_latest_orderbooks(
    const std::vector<std::string>& symbols, CryptoLoc loc) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/latest/orderbooks";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols);

    json j = httpClient.get(qb.build());

    std::map<std::string, CryptoOrderbook> result;
    if (j.contains("orderbooks") && j["orderbooks"].is_object()) {
        for (auto& [sym, ob_json] : j["orderbooks"].items()) {
            result.emplace(sym, CryptoOrderbook(ob_json, sym));
        }
    }
    return result;
}

std::map<std::string, CryptoSnapshot> AlpacaMarketDataAPI::get_crypto_snapshots(
    const std::vector<std::string>& symbols, CryptoLoc loc) {

    std::string base = "/v1beta3/crypto/" + crypto_loc_to_string(loc) + "/snapshots";
    auto qb = QueryBuilder(base)
        .add_list("symbols", symbols);

    json j = httpClient.get(qb.build());

    std::map<std::string, CryptoSnapshot> result;
    if (j.contains("snapshots") && j["snapshots"].is_object()) {
        for (auto& [sym, snapshot_json] : j["snapshots"].items()) {
            result.emplace(sym, CryptoSnapshot(snapshot_json, sym));
        }
    }
    return result;
}

// =====================================================================
//  FOREX ENDPOINTS
// =====================================================================

std::map<std::string, ForexRate> AlpacaMarketDataAPI::get_forex_latest_rates(
    const std::vector<std::string>& currency_pairs) {

    auto qb = QueryBuilder("/v1beta1/forex/latest/rates")
        .add_list("currency_pairs", currency_pairs);

    json j = httpClient.get(qb.build());

    std::map<std::string, ForexRate> result;
    if (j.contains("rates") && j["rates"].is_object()) {
        for (auto& [pair, rate_json] : j["rates"].items()) {
            result.emplace(pair, ForexRate(rate_json, pair));
        }
    }
    return result;
}

std::map<std::string, std::vector<ForexRate>> AlpacaMarketDataAPI::get_forex_rates(
    const std::vector<std::string>& currency_pairs,
    DataTimeframe timeframe,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v1beta1/forex/rates")
        .add_list("currency_pairs", currency_pairs)
        .add("timeframe", data_timeframe_to_string(timeframe))
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<std::map<std::string, std::vector<ForexRate>>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            if (!j.contains("rates") || !j["rates"].is_object()) {
                return;
            }
            for (const auto& [pair, rates_json] : j["rates"].items()) {
                auto& rates = result[pair];
                rates.reserve(rates.size() + rates_json.size());
                for (const auto& rate_json : rates_json) {
                    rates.emplace_back(rate_json, pair);
                }
            }
        });
}

// =====================================================================
//  FIXED INCOME
// =====================================================================

std::map<std::string, FixedIncomePrice> AlpacaMarketDataAPI::get_fixed_income_latest_prices(
    const std::vector<std::string>& symbols) {

    auto qb = QueryBuilder("/v1beta1/fixed_income/latest/prices")
        .add_list("symbols", symbols);

    json j = httpClient.get(qb.build());

    std::map<std::string, FixedIncomePrice> result;
    if (j.contains("prices") && j["prices"].is_object()) {
        for (auto& [sym, price_json] : j["prices"].items()) {
            result.emplace(sym, FixedIncomePrice(price_json, sym));
        }
    }
    return result;
}

// =====================================================================
//  LOGOS
// =====================================================================

std::string AlpacaMarketDataAPI::get_logo_url(const std::string& symbol) {
    // The logos endpoint returns an image redirect; construct the URL for direct use
    return httpClient.get_base_url() + "/v1beta1/logos/" + url_encode(symbol);
}

// =====================================================================
//  SCREENER
// =====================================================================

std::vector<MostActive> AlpacaMarketDataAPI::get_most_actives(int top) {
    auto qb = QueryBuilder("/v1beta1/screener/stocks/most-actives")
        .add("top", top);

    json j = httpClient.get(qb.build());

    std::vector<MostActive> result;
    if (j.contains("most_actives") && j["most_actives"].is_array()) {
        result = parse_array<MostActive>(j["most_actives"]);
    }
    return result;
}

std::vector<Mover> AlpacaMarketDataAPI::get_movers(ScreenerMarketType market_type, int top) {
    auto qb = QueryBuilder("/v1beta1/screener/" + screener_market_type_to_string(market_type) + "/movers")
        .add("top", top);

    json j = httpClient.get(qb.build());

    std::vector<Mover> result;
    // Response has "gainers" and "losers" sub-arrays
    for (const char* key : {"gainers", "losers"}) {
        if (j.contains(key) && j[key].is_array()) {
            auto sub = parse_array<Mover>(j[key]);
            result.insert(result.end(), sub.begin(), sub.end());
        }
    }
    return result;
}

// =====================================================================
//  NEWS
// =====================================================================

std::vector<NewsArticle> AlpacaMarketDataAPI::get_news(
    const std::vector<std::string>& symbols,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort,
    bool include_content) {

    auto qb = QueryBuilder("/v1beta1/news")
        .add_list("symbols", symbols)
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    if (include_content) qb.add("include_content", true);

    return collect_paginated<std::vector<NewsArticle>>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, auto& result) {
            merge_array_page<NewsArticle>(j, "news", result);
        });
}

// =====================================================================
//  CORPORATE ACTIONS
// =====================================================================

json AlpacaMarketDataAPI::get_corporate_actions(
    const std::vector<std::string>& symbols,
    const std::vector<std::string>& types,
    const std::string& start, const std::string& end,
    int limit, const std::string& page_token, Sort sort) {

    auto qb = QueryBuilder("/v1/corporate-actions")
        .add_list("symbols", symbols)
        .add_list("types", types)
        .add("start", start)
        .add("end", end)
        .add("limit", limit)
        .add("sort", sort_to_string(sort));

    return collect_paginated<json>(
        qb.build(),
        page_token,
        [this](const std::string& endpoint) { return get_json(endpoint); },
        [](const json& j, json& result) {
            merge_array_field_page(j, "corporate_actions", result);
        });
}

} // namespace alpaca
