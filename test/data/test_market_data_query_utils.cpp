#include <gtest/gtest.h>

#include "alpaca/core/util.h"

using namespace alpaca;

TEST(OptionFeedUtilTest, ConvertsSupportedOptionFeeds) {
    EXPECT_EQ(option_feed_to_string(OptionFeed::INDICATIVE), "indicative");
    EXPECT_EQ(option_feed_to_string(OptionFeed::OPRA), "opra");
    EXPECT_TRUE(option_feed_to_string(OptionFeed::DEFAULT).empty());
}

TEST(QueryBuilderTest, OmitsDefaultOptionFeedQueryParameter) {
    const auto query = QueryBuilder("/v1beta1/options/snapshots")
        .add_list("symbols", {"AAPL250321C00200000"})
        .add("feed", option_feed_to_string(OptionFeed::DEFAULT))
        .build();

    EXPECT_EQ(query, "/v1beta1/options/snapshots?symbols=AAPL250321C00200000");
}

TEST(QueryBuilderTest, IncludesExplicitOptionFeedQueryParameter) {
    const auto query = QueryBuilder("/v1beta1/options/snapshots")
        .add_list("symbols", {"AAPL250321C00200000"})
        .add("feed", option_feed_to_string(OptionFeed::INDICATIVE))
        .build();

    EXPECT_EQ(query, "/v1beta1/options/snapshots?symbols=AAPL250321C00200000&feed=indicative");
}
