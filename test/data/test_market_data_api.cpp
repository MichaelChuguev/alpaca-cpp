#include <gtest/gtest.h>

#include "alpaca/AlpacaMarketDataAPI.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_trade_json(const std::string& timestamp, const std::string& price) {
    json j;
    j["t"] = timestamp;
    j["p"] = price;
    j["s"] = "1";
    j["x"] = "C";
    j["c"] = "I";
    return j;
}

json make_quote_json(const std::string& timestamp, const std::string& ask_price) {
    json j;
    j["t"] = timestamp;
    j["ap"] = ask_price;
    j["as"] = "1";
    j["ax"] = "C";
    j["bp"] = "0.01";
    j["bs"] = "1";
    j["bx"] = "N";
    j["c"] = "A";
    return j;
}

json make_bar_json(
    const std::string& timestamp,
    const std::string& close,
    const std::string& symbol = "") {

    json j;
    j["t"] = timestamp;
    j["o"] = close;
    j["h"] = close;
    j["l"] = close;
    j["c"] = close;
    j["v"] = "1";
    j["n"] = 1;
    j["vw"] = close;
    if (!symbol.empty()) j["S"] = symbol;
    return j;
}

json make_snapshot_json(
    const std::string& timestamp,
    const std::string& price,
    const std::string& ask_price,
    const std::string& iv = "",
    const json& greeks = nullptr) {

    json j;
    j["latestTrade"] = make_trade_json(timestamp, price);
    j["latestQuote"] = make_quote_json(timestamp, ask_price);
    if (!iv.empty()) j["impliedVolatility"] = iv;
    if (!greeks.is_null()) j["greeks"] = greeks;
    return j;
}

json make_news_json(
    uint64_t id,
    const std::string& headline,
    const std::string& timestamp) {

    json j;
    j["id"] = id;
    j["headline"] = headline;
    j["author"] = "Wire";
    j["created_at"] = timestamp;
    j["updated_at"] = timestamp;
    j["summary"] = "summary";
    j["content"] = "content";
    j["url"] = "https://example.com/story";
    j["source"] = "Example";
    j["images"] = json::array();
    j["symbols"] = json::array({"AAPL"});
    return j;
}

class StubMarketDataAPI : public AlpacaMarketDataAPI {
public:
    std::vector<std::string> requests;
    std::vector<json> responses;

    StubMarketDataAPI()
        : AlpacaMarketDataAPI("test-key", "test-secret", MarketDataEndpoint::LIVE)
    {}

protected:
    json get_json(const std::string& endpoint) override {
        requests.push_back(endpoint);
        if (requests.size() > responses.size()) {
            ADD_FAILURE() << "Unexpected GET request: " << endpoint;
            return json::object();
        }
        return responses[requests.size() - 1];
    }
};

} // namespace

TEST(MarketDataAPITest, OptionChainAggregatesAllPages) {
    StubMarketDataAPI api;

    json greeks;
    greeks["delta"] = "-0.0208";
    greeks["gamma"] = "0.0046";
    greeks["theta"] = "-0.0678";
    greeks["vega"] = "0.0133";
    greeks["rho"] = "-0.0006";

    json page1;
    page1["snapshots"] = json::object();
    page1["snapshots"]["AAPL260402C00200000"] =
        make_snapshot_json("2026-04-01T19:59:59Z", "0.01", "0.03");
    page1["next_page_token"] = "token+/=";

    json page2;
    page2["snapshots"] = json::object();
    page2["snapshots"]["AAPL260406P00232500"] =
        make_snapshot_json("2026-04-01T19:40:43Z", "0.03", "0.02", "0.4116", greeks);

    api.responses = {page1, page2};

    const auto chain = api.get_option_chain("AAPL", OptionFeed::INDICATIVE);

    ASSERT_EQ(api.requests.size(), 2u);
    EXPECT_EQ(api.requests[0], "/v1beta1/options/snapshots/AAPL?feed=indicative");
    EXPECT_EQ(
        api.requests[1],
        "/v1beta1/options/snapshots/AAPL?feed=indicative&page_token=token%2B%2F%3D");

    ASSERT_EQ(chain.size(), 2u);
    EXPECT_EQ(chain.at("AAPL260402C00200000").latest_trade.price, Decimal(0.01));
    EXPECT_EQ(chain.at("AAPL260406P00232500").implied_volatility, Decimal(0.4116));
    EXPECT_EQ(chain.at("AAPL260406P00232500").greeks_delta, Decimal(-0.0208));
}

TEST(MarketDataAPITest, OptionChainRawJsonAggregatesAllPages) {
    StubMarketDataAPI api;

    json page1;
    page1["snapshots"] = json::object();
    page1["snapshots"]["AAPL260402P00155000"] =
        make_snapshot_json("2026-02-13T15:45:05Z", "0.5", "0.01");
    page1["next_page_token"] = "next-token";

    json page2;
    page2["snapshots"] = json::object();
    page2["snapshots"]["AAPL260406C00230000"] =
        make_snapshot_json("2026-04-01T19:25:08Z", "0.03", "0.04", "0.2871");

    api.responses = {page1, page2};

    const json raw = api.get_option_chain_raw_json("AAPL", OptionFeed::INDICATIVE, "", 50);

    ASSERT_EQ(api.requests.size(), 2u);
    EXPECT_EQ(api.requests[0], "/v1beta1/options/snapshots/AAPL?feed=indicative&limit=50");
    EXPECT_EQ(
        api.requests[1],
        "/v1beta1/options/snapshots/AAPL?feed=indicative&limit=50&page_token=next-token");

    ASSERT_TRUE(raw.contains("snapshots"));
    ASSERT_TRUE(raw["snapshots"].is_object());
    EXPECT_FALSE(raw.contains("next_page_token"));
    EXPECT_EQ(raw["snapshots"].size(), 2u);
    EXPECT_EQ(raw["snapshots"]["AAPL260406C00230000"]["impliedVolatility"], "0.2871");
}

TEST(MarketDataAPITest, StockBarsAggregateAllPages) {
    StubMarketDataAPI api;

    json page1;
    page1["bars"] = json::object();
    page1["bars"]["AAPL"] = json::array({
        make_bar_json("2026-04-01T00:00:00Z", "100.00")
    });
    page1["next_page_token"] = "bars+/=";

    json page2;
    page2["bars"] = json::object();
    page2["bars"]["MSFT"] = json::array({
        make_bar_json("2026-04-01T00:00:00Z", "200.00")
    });

    api.responses = {page1, page2};

    const auto bars = api.get_stock_bars({"AAPL", "MSFT"}, DataTimeframe::ONE_DAY, "", "", DataFeed::IEX, "", 1);

    ASSERT_EQ(api.requests.size(), 2u);
    EXPECT_EQ(
        api.requests[0],
        "/v2/stocks/bars?symbols=AAPL,MSFT&timeframe=1Day&feed=iex&limit=1&sort=asc");
    EXPECT_EQ(
        api.requests[1],
        "/v2/stocks/bars?symbols=AAPL,MSFT&timeframe=1Day&feed=iex&limit=1&sort=asc&page_token=bars%2B%2F%3D");

    ASSERT_EQ(bars.size(), 2u);
    EXPECT_EQ(bars.at("AAPL").front().close, Decimal(100.00));
    EXPECT_EQ(bars.at("AAPL").front().symbol, "AAPL");
    EXPECT_EQ(bars.at("MSFT").front().close, Decimal(200.00));
    EXPECT_EQ(bars.at("MSFT").front().symbol, "MSFT");
}

TEST(MarketDataAPITest, StockBarsSingleBackfillsRequestedSymbol) {
    StubMarketDataAPI api;

    json page1;
    page1["bars"] = json::array({
        make_bar_json("2026-04-01T00:00:00Z", "100.00"),
        make_bar_json("2026-04-02T00:00:00Z", "101.00")
    });

    api.responses = {page1};

    const auto bars = api.get_stock_bars_single(
        "AAPL",
        DataTimeframe::ONE_DAY,
        "",
        "",
        DataFeed::IEX,
        "",
        2);

    ASSERT_EQ(api.requests.size(), 1u);
    EXPECT_EQ(
        api.requests[0],
        "/v2/stocks/AAPL/bars?timeframe=1Day&feed=iex&limit=2&sort=asc");

    ASSERT_EQ(bars.size(), 2u);
    EXPECT_EQ(bars[0].symbol, "AAPL");
    EXPECT_EQ(bars[1].symbol, "AAPL");
}

TEST(MarketDataAPITest, StockLatestBarsBackfillSymbols) {
    StubMarketDataAPI api;

    json response;
    response["bars"] = json::object();
    response["bars"]["AAPL"] = make_bar_json("2026-04-02T00:00:00Z", "101.00");
    response["bars"]["MSFT"] = make_bar_json("2026-04-02T00:00:00Z", "202.00");

    api.responses = {response};

    const auto bars = api.get_stock_latest_bars({"AAPL", "MSFT"}, DataFeed::IEX);

    ASSERT_EQ(api.requests.size(), 1u);
    EXPECT_EQ(api.requests[0], "/v2/stocks/bars/latest?symbols=AAPL,MSFT&feed=iex");

    ASSERT_EQ(bars.size(), 2u);
    EXPECT_EQ(bars.at("AAPL").symbol, "AAPL");
    EXPECT_EQ(bars.at("MSFT").symbol, "MSFT");
}

TEST(MarketDataAPITest, StockLatestBarSingleBackfillsRequestedSymbol) {
    StubMarketDataAPI api;

    json response;
    response["bar"] = make_bar_json("2026-04-02T00:00:00Z", "101.00");

    api.responses = {response};

    const auto bar = api.get_stock_latest_bar_single("AAPL", DataFeed::IEX);

    ASSERT_EQ(api.requests.size(), 1u);
    EXPECT_EQ(api.requests[0], "/v2/stocks/AAPL/bars/latest?feed=iex");
    EXPECT_EQ(bar.symbol, "AAPL");
}

TEST(MarketDataAPITest, OptionSnapshotsAggregateAllPages) {
    StubMarketDataAPI api;

    json page1;
    page1["snapshots"] = json::object();
    page1["snapshots"]["AAPL260402C00200000"] =
        make_snapshot_json("2026-04-01T19:59:59Z", "0.01", "0.03");
    page1["next_page_token"] = "snapshot token";

    json page2;
    page2["snapshots"] = json::object();
    page2["snapshots"]["AAPL260406P00232500"] =
        make_snapshot_json("2026-04-01T19:40:43Z", "0.03", "0.02", "0.4116");

    api.responses = {page1, page2};

    const auto snapshots = api.get_option_snapshots(
        {"AAPL260402C00200000", "AAPL260406P00232500"},
        OptionFeed::INDICATIVE,
        1);

    ASSERT_EQ(api.requests.size(), 2u);
    EXPECT_EQ(
        api.requests[0],
        "/v1beta1/options/snapshots?symbols=AAPL260402C00200000,AAPL260406P00232500&feed=indicative&limit=1");
    EXPECT_EQ(
        api.requests[1],
        "/v1beta1/options/snapshots?symbols=AAPL260402C00200000,AAPL260406P00232500&feed=indicative&limit=1&page_token=snapshot%20token");

    ASSERT_EQ(snapshots.size(), 2u);
    EXPECT_EQ(snapshots.at("AAPL260406P00232500").implied_volatility, Decimal(0.4116));
}

TEST(MarketDataAPITest, NewsAggregatesAllPages) {
    StubMarketDataAPI api;

    json page1;
    page1["news"] = json::array({
        make_news_json(1, "First", "2026-04-01T10:00:00Z")
    });
    page1["next_page_token"] = "news-token";

    json page2;
    page2["news"] = json::array({
        make_news_json(2, "Second", "2026-04-01T09:00:00Z")
    });

    api.responses = {page1, page2};

    const auto news = api.get_news({"AAPL"}, "", "", 1);

    ASSERT_EQ(api.requests.size(), 2u);
    EXPECT_EQ(api.requests[0], "/v1beta1/news?symbols=AAPL&limit=1&sort=desc");
    EXPECT_EQ(api.requests[1], "/v1beta1/news?symbols=AAPL&limit=1&sort=desc&page_token=news-token");

    ASSERT_EQ(news.size(), 2u);
    EXPECT_EQ(news[0].headline, "First");
    EXPECT_EQ(news[1].headline, "Second");
}

TEST(MarketDataAPITest, CorporateActionsAggregateAllPages) {
    StubMarketDataAPI api;

    json page1;
    page1["corporate_actions"] = json::array({
        json{{"id", "action-1"}, {"symbol", "AAPL"}}
    });
    page1["next_page_token"] = "corp-token";

    json page2;
    page2["corporate_actions"] = json::array({
        json{{"id", "action-2"}, {"symbol", "MSFT"}}
    });

    api.responses = {page1, page2};

    const json actions = api.get_corporate_actions({"AAPL", "MSFT"}, {}, "2026-04-01", "2026-04-02", 1);

    ASSERT_EQ(api.requests.size(), 2u);
    EXPECT_EQ(
        api.requests[0],
        "/v1/corporate-actions?symbols=AAPL,MSFT&start=2026-04-01&end=2026-04-02&limit=1&sort=asc");
    EXPECT_EQ(
        api.requests[1],
        "/v1/corporate-actions?symbols=AAPL,MSFT&start=2026-04-01&end=2026-04-02&limit=1&sort=asc&page_token=corp-token");

    ASSERT_TRUE(actions.contains("corporate_actions"));
    ASSERT_TRUE(actions["corporate_actions"].is_array());
    EXPECT_FALSE(actions.contains("next_page_token"));
    ASSERT_EQ(actions["corporate_actions"].size(), 2u);
    EXPECT_EQ(actions["corporate_actions"][0]["id"], "action-1");
    EXPECT_EQ(actions["corporate_actions"][1]["id"], "action-2");
}
