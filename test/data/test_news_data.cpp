#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/model/data/news.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_news_article_json() {
    json j;
    j["id"] = 12345678;
    j["headline"] = "Apple Reports Record Earnings";
    j["author"] = "John Doe";
    j["created_at"] = "2026-03-10T14:30:00Z";
    j["updated_at"] = "2026-03-10T15:00:00Z";
    j["summary"] = "Apple beat expectations with strong iPhone sales";
    j["content"] = "<p>Full article content here...</p>";
    j["url"] = "https://example.com/article/12345";
    j["source"] = "Bloomberg";
    j["images"] = json::array();
    json img;
    img["url"] = "https://example.com/img/thumb.jpg";
    img["size"] = "thumb";
    j["images"].push_back(img);
    json img2;
    img2["url"] = "https://example.com/img/large.jpg";
    img2["size"] = "large";
    j["images"].push_back(img2);
    j["symbols"] = json::array({"AAPL", "MSFT"});
    return j;
}

json make_most_active_json() {
    json j;
    j["symbol"] = "TSLA";
    j["volume"] = "85000000";
    j["trade_count"] = "1200000";
    return j;
}

json make_mover_json() {
    json j;
    j["symbol"] = "NVDA";
    j["price"] = "950.50";
    j["change"] = "45.25";
    j["percent_change"] = "5.0";
    j["volume"] = "60000000";
    return j;
}

json make_forex_rate_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["bp"] = "0.8950";
    j["ap"] = "0.8955";
    j["mp"] = "0.89525";
    return j;
}

json make_fixed_income_price_json() {
    json j;
    j["t"] = "2026-03-10T14:30:00Z";
    j["r"] = "4.325";
    return j;
}

json make_corporate_action_json() {
    json j;
    j["id"] = "ca_123456";
    j["type"] = "dividend";
    j["symbol"] = "AAPL";
    j["date"] = "2026-03-15T00:00:00Z";
    j["description"] = "Quarterly cash dividend";
    j["cash"] = "0.24";
    j["old_rate"] = "0";
    j["new_rate"] = "0";
    j["process_date"] = "2026-03-14";
    j["payable_date"] = "2026-03-20";
    j["record_date"] = "2026-03-16";
    j["ex_date"] = "2026-03-14";
    return j;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// NewsImage tests
// ---------------------------------------------------------------------------

TEST(NewsImageTest, ParseFromJson) {
    json j;
    j["url"] = "https://example.com/img.jpg";
    j["size"] = "thumb";
    NewsImage img(j);

    EXPECT_EQ(img.url, "https://example.com/img.jpg");
    EXPECT_EQ(img.size, "thumb");
}

// ---------------------------------------------------------------------------
// NewsArticle tests
// ---------------------------------------------------------------------------

TEST(NewsArticleTest, ParseFromJson) {
    std::cout << "\n=== Test: NewsArticleTest::ParseFromJson ===" << std::endl;

    json j = make_news_article_json();
    NewsArticle article(j);

    EXPECT_EQ(article.id, 12345678u);
    EXPECT_EQ(article.headline, "Apple Reports Record Earnings");
    EXPECT_EQ(article.author, "John Doe");
    EXPECT_EQ(article.summary, "Apple beat expectations with strong iPhone sales");
    EXPECT_EQ(article.content, "<p>Full article content here...</p>");
    EXPECT_EQ(article.url, "https://example.com/article/12345");
    EXPECT_EQ(article.source, "Bloomberg");
    ASSERT_EQ(article.images.size(), 2u);
    EXPECT_EQ(article.images[0].url, "https://example.com/img/thumb.jpg");
    EXPECT_EQ(article.images[0].size, "thumb");
    EXPECT_EQ(article.images[1].size, "large");
    ASSERT_EQ(article.symbols.size(), 2u);
    EXPECT_EQ(article.symbols[0], "AAPL");
    EXPECT_EQ(article.symbols[1], "MSFT");

    std::cout << "PASSED" << std::endl;
}

TEST(NewsArticleTest, ParseMissingOptionalFields) {
    json j;
    j["headline"] = "Test";
    j["author"] = "Test Author";
    j["created_at"] = "2026-03-10T00:00:00Z";
    j["updated_at"] = "2026-03-10T00:00:00Z";
    j["summary"] = "";
    j["content"] = "";
    j["url"] = "";
    j["source"] = "test";
    // No id, images, symbols

    NewsArticle article(j);
    EXPECT_EQ(article.id, 0u);
    EXPECT_TRUE(article.images.empty());
    EXPECT_TRUE(article.symbols.empty());
}

// ---------------------------------------------------------------------------
// MostActive tests
// ---------------------------------------------------------------------------

TEST(MostActiveTest, ParseFromJson) {
    std::cout << "\n=== Test: MostActiveTest::ParseFromJson ===" << std::endl;

    json j = make_most_active_json();
    MostActive active(j);

    EXPECT_EQ(active.symbol, "TSLA");
    EXPECT_EQ(active.volume, Decimal(85000000));
    EXPECT_EQ(active.trade_count, Decimal(1200000));

    std::cout << "PASSED" << std::endl;
}

// ---------------------------------------------------------------------------
// Mover tests
// ---------------------------------------------------------------------------

TEST(MoverTest, ParseFromJson) {
    std::cout << "\n=== Test: MoverTest::ParseFromJson ===" << std::endl;

    json j = make_mover_json();
    Mover mover(j);

    EXPECT_EQ(mover.symbol, "NVDA");
    EXPECT_EQ(mover.price, Decimal(950.50));
    EXPECT_EQ(mover.change, Decimal(45.25));
    EXPECT_EQ(mover.percent_change, Decimal(5.0));
    EXPECT_EQ(mover.volume, Decimal(60000000));

    std::cout << "PASSED" << std::endl;
}

// ---------------------------------------------------------------------------
// CorporateAction tests
// ---------------------------------------------------------------------------

TEST(CorporateActionTest, ParseFromJson) {
    std::cout << "\n=== Test: CorporateActionTest::ParseFromJson ===" << std::endl;

    json j = make_corporate_action_json();
    CorporateAction action(j);

    EXPECT_EQ(action.id, "ca_123456");
    EXPECT_EQ(action.corporate_actions_type, "dividend");
    EXPECT_EQ(action.symbol, "AAPL");
    EXPECT_EQ(action.description, "Quarterly cash dividend");
    EXPECT_EQ(action.cash, Decimal(0.24));
    EXPECT_EQ(action.process_date, "2026-03-14");
    EXPECT_EQ(action.payable_date, "2026-03-20");
    EXPECT_EQ(action.record_date, "2026-03-16");
    EXPECT_EQ(action.ex_date, "2026-03-14");

    std::cout << "PASSED" << std::endl;
}

// ---------------------------------------------------------------------------
// ForexRate tests
// ---------------------------------------------------------------------------

TEST(ForexRateTest, ParseFromJson) {
    std::cout << "\n=== Test: ForexRateTest::ParseFromJson ===" << std::endl;

    json j = make_forex_rate_json();
    ForexRate rate(j, "USD/EUR");

    EXPECT_EQ(rate.currency_pair, "USD/EUR");
    EXPECT_EQ(rate.bid_price, Decimal(0.8950));
    EXPECT_EQ(rate.ask_price, Decimal(0.8955));
    EXPECT_EQ(rate.mid_price, Decimal(0.89525));

    std::cout << "PASSED" << std::endl;
}

TEST(ForexRateTest, DefaultPair) {
    json j = make_forex_rate_json();
    ForexRate rate(j);

    EXPECT_TRUE(rate.currency_pair.empty());
    EXPECT_EQ(rate.bid_price, Decimal(0.8950));
}

// ---------------------------------------------------------------------------
// FixedIncomePrice tests
// ---------------------------------------------------------------------------

TEST(FixedIncomePriceTest, ParseFromJson) {
    std::cout << "\n=== Test: FixedIncomePriceTest::ParseFromJson ===" << std::endl;

    json j = make_fixed_income_price_json();
    FixedIncomePrice price(j, "US10Y");

    EXPECT_EQ(price.symbol, "US10Y");
    EXPECT_EQ(price.rate, Decimal(4.325));

    std::cout << "PASSED" << std::endl;
}

TEST(FixedIncomePriceTest, DefaultSymbol) {
    json j = make_fixed_income_price_json();
    FixedIncomePrice price(j);

    EXPECT_TRUE(price.symbol.empty());
    EXPECT_EQ(price.rate, Decimal(4.325));
}
