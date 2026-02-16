#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/asset.h"

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
    j["attributes"] = json::array({"has_options", "ipo"});
    return j;
}

} // anonymous namespace

TEST(AssetTest, ParseFromJson) {
    json j = make_asset_json();
    Asset asset(j);

    EXPECT_EQ(asset.id, "b0b6dd9d-8b9b-48a9-ba46-b9d54906e415");
    EXPECT_EQ(asset.asset_class, "us_equity");
    EXPECT_EQ(asset.exchange, "NASDAQ");
    EXPECT_EQ(asset.symbol, "AAPL");
    EXPECT_EQ(asset.name, "Apple Inc. Common Stock");
    EXPECT_EQ(asset.status, "active");
    EXPECT_TRUE(asset.tradable);
    EXPECT_TRUE(asset.marginable);
    EXPECT_DOUBLE_EQ(asset.margin_requirement_long, 25.0);
    EXPECT_DOUBLE_EQ(asset.margin_requirement_short, 25.0);
    EXPECT_TRUE(asset.shortable);
    EXPECT_TRUE(asset.easy_to_borrow);
    EXPECT_TRUE(asset.fractionable);
    ASSERT_EQ(asset.attributes.size(), 2u);
    EXPECT_EQ(asset.attributes[0], "has_options");
    EXPECT_EQ(asset.attributes[1], "ipo");
}

TEST(AssetTest, ParseNullAttributes) {
    json j = make_asset_json();
    j["attributes"] = nullptr;
    Asset asset(j);

    EXPECT_TRUE(asset.attributes.empty());
}

TEST(AssetTest, ParseMissingAttributes) {
    json j = make_asset_json();
    j.erase("attributes");
    Asset asset(j);

    EXPECT_TRUE(asset.attributes.empty());
}

TEST(AssetTest, ToJson) {
    json j = make_asset_json();
    Asset asset(j);
    json result = asset.to_json();

    EXPECT_EQ(result["id"], "b0b6dd9d-8b9b-48a9-ba46-b9d54906e415");
    EXPECT_EQ(result["class"], "us_equity");
    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_TRUE(result["tradable"].get<bool>());
}

TEST(AssetTest, ToString) {
    json j = make_asset_json();
    Asset asset(j);
    std::string str = asset.to_string();

    EXPECT_NE(str.find("Asset Information:"), std::string::npos);
    EXPECT_NE(str.find("AAPL"), std::string::npos);
    EXPECT_NE(str.find("NASDAQ"), std::string::npos);
}

TEST(AssetTest, EmptyAttributesToJsonNull) {
    json j = make_asset_json();
    j["attributes"] = nullptr;
    Asset asset(j);
    json result = asset.to_json();

    EXPECT_TRUE(result["attributes"].is_null());
}
