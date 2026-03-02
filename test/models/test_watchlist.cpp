#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/watchlist.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_asset_json() {
    json j;
    j["id"] = "b0b6dd9d-8b9b-48a9-ba46-b9d54906e415";
    j["class"] = "us_equity";
    j["exchange"] = "NASDAQ";
    j["symbol"] = "AAPL";
    j["name"] = "Apple Inc. Common Stock";
    j["status"] = "active";
    j["tradable"] = true;
    j["marginable"] = true;
    j["margin_requirement_long"] = "25";
    j["margin_requirement_short"] = "25";
    j["shortable"] = true;
    j["easy_to_borrow"] = true;
    j["fractionable"] = true;
    j["attributes"] = nullptr;
    return j;
}

json make_watchlist_json(bool with_assets = true) {
    json j;
    j["id"] = "3174d6df-7726-44b4-a5bd-7fda5ae6e009";
    j["account_id"] = "abe25343-a7ba-4255-bdeb-f7e013e9ee5d";
    j["created_at"] = "2022-01-31T21:49:05.14628Z";
    j["updated_at"] = "2022-01-31T21:49:05.14628Z";
    j["name"] = "Primary Watchlist";
    if (with_assets) {
        j["assets"] = json::array({make_asset_json()});
    }
    return j;
}

} // anonymous namespace

TEST(WatchlistTest, ParseFromJson) {
    json j = make_watchlist_json();
    Watchlist wl(j);

    EXPECT_EQ(wl.id, "3174d6df-7726-44b4-a5bd-7fda5ae6e009");
    EXPECT_EQ(wl.account_id, "abe25343-a7ba-4255-bdeb-f7e013e9ee5d");
    EXPECT_EQ(wl.name, "Primary Watchlist");
    ASSERT_EQ(wl.assets.size(), 1u);
    EXPECT_EQ(wl.assets[0].symbol, "AAPL");
}

TEST(WatchlistTest, ParseWithoutAssets) {
    json j = make_watchlist_json(false);
    Watchlist wl(j);

    EXPECT_EQ(wl.id, "3174d6df-7726-44b4-a5bd-7fda5ae6e009");
    EXPECT_EQ(wl.name, "Primary Watchlist");
    EXPECT_TRUE(wl.assets.empty());
}

TEST(WatchlistTest, ParseMultipleAssets) {
    json j = make_watchlist_json();
    json asset2 = make_asset_json();
    asset2["symbol"] = "TSLA";
    asset2["name"] = "Tesla Inc.";
    asset2["id"] = "8ccae427-5dd0-45b3-b5fe-7ba5e422c766";
    j["assets"].push_back(asset2);

    Watchlist wl(j);

    ASSERT_EQ(wl.assets.size(), 2u);
    EXPECT_EQ(wl.assets[0].symbol, "AAPL");
    EXPECT_EQ(wl.assets[1].symbol, "TSLA");
}

TEST(WatchlistTest, ToString) {
    json j = make_watchlist_json();
    Watchlist wl(j);
    std::string str = wl.to_string();

    EXPECT_NE(str.find("Watchlist:"), std::string::npos);
    EXPECT_NE(str.find("Primary Watchlist"), std::string::npos);
    EXPECT_NE(str.find("AAPL"), std::string::npos);
}

TEST(WatchlistTest, NullAssets) {
    json j = make_watchlist_json(false);
    j["assets"] = nullptr;
    Watchlist wl(j);

    EXPECT_TRUE(wl.assets.empty());
}

TEST(WatchlistTest, ParseFromJsonArray) {
    json arr = json::array({make_watchlist_json(false), make_watchlist_json(false)});
    arr[1]["name"] = "Secondary Watchlist";

    std::vector<Watchlist> watchlists;
    for (const auto& wl_json : arr) {
        watchlists.push_back(Watchlist(wl_json));
    }

    ASSERT_EQ(watchlists.size(), 2u);
    EXPECT_EQ(watchlists[0].name, "Primary Watchlist");
    EXPECT_EQ(watchlists[1].name, "Secondary Watchlist");
}
