#include <gtest/gtest.h>
#include <iostream>
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
    std::cout << "\n=== Test: AssetTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: GET /v2/assets/{symbol} - Parse asset response" << std::endl;

    json j = make_asset_json();
    Asset asset(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  ID: " << asset.id << std::endl;
    std::cout << "  Symbol: " << asset.symbol << std::endl;
    std::cout << "  Name: " << asset.name << std::endl;
    std::cout << "  Class: " << asset.asset_class << std::endl;
    std::cout << "  Exchange: " << asset.exchange << std::endl;
    std::cout << "  Status: " << asset.status << std::endl;
    std::cout << "  Tradable: " << (asset.tradable ? "true" : "false") << std::endl;
    std::cout << "  Shortable: " << (asset.shortable ? "true" : "false") << std::endl;
    std::cout << "  Fractionable: " << (asset.fractionable ? "true" : "false") << std::endl;
    std::cout << "  Attributes: " << asset.attributes.size() << " items" << std::endl;

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

    std::cout << "PASSED" << std::endl;
}

TEST(AssetTest, ParseNullAttributes) {
    std::cout << "\n=== Test: AssetTest::ParseNullAttributes ===" << std::endl;
    std::cout << "Testing: GET /v2/assets - Handle null attributes field" << std::endl;

    json j = make_asset_json();
    j["attributes"] = nullptr;
    Asset asset(j);

    std::cout << "Response: attributes is null -> empty vector (size=" << asset.attributes.size() << ")" << std::endl;

    EXPECT_TRUE(asset.attributes.empty());

    std::cout << "PASSED" << std::endl;
}

TEST(AssetTest, ParseMissingAttributes) {
    std::cout << "\n=== Test: AssetTest::ParseMissingAttributes ===" << std::endl;
    std::cout << "Testing: GET /v2/assets - Handle missing attributes field" << std::endl;

    json j = make_asset_json();
    j.erase("attributes");
    Asset asset(j);

    std::cout << "Response: attributes missing -> empty vector (size=" << asset.attributes.size() << ")" << std::endl;

    EXPECT_TRUE(asset.attributes.empty());

    std::cout << "PASSED" << std::endl;
}

TEST(AssetTest, ToJson) {
    std::cout << "\n=== Test: AssetTest::ToJson ===" << std::endl;
    std::cout << "Testing: Asset model to_json() round-trip" << std::endl;

    json j = make_asset_json();
    Asset asset(j);
    json result = asset.to_json();

    std::cout << "Serialized JSON: " << result.dump(2) << std::endl;

    EXPECT_EQ(result["id"], "b0b6dd9d-8b9b-48a9-ba46-b9d54906e415");
    EXPECT_EQ(result["class"], "us_equity");
    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_TRUE(result["tradable"].get<bool>());

    std::cout << "PASSED" << std::endl;
}

TEST(AssetTest, ToString) {
    std::cout << "\n=== Test: AssetTest::ToString ===" << std::endl;
    std::cout << "Testing: Asset model to_string() display" << std::endl;

    json j = make_asset_json();
    Asset asset(j);
    std::string str = asset.to_string();

    std::cout << "Response:\n" << str << std::endl;

    EXPECT_NE(str.find("Asset Information:"), std::string::npos);
    EXPECT_NE(str.find("AAPL"), std::string::npos);
    EXPECT_NE(str.find("NASDAQ"), std::string::npos);

    std::cout << "PASSED" << std::endl;
}

TEST(AssetTest, EmptyAttributesToJsonNull) {
    std::cout << "\n=== Test: AssetTest::EmptyAttributesToJsonNull ===" << std::endl;
    std::cout << "Testing: Asset with null attributes serializes correctly" << std::endl;

    json j = make_asset_json();
    j["attributes"] = nullptr;
    Asset asset(j);
    json result = asset.to_json();

    std::cout << "Attributes in JSON: " << result["attributes"].dump() << std::endl;

    EXPECT_TRUE(result["attributes"].is_null());

    std::cout << "PASSED" << std::endl;
}

TEST(AssetTest, ParseAssetArray) {
    std::cout << "\n=== Test: AssetTest::ParseAssetArray ===" << std::endl;
    std::cout << "Testing: GET /v2/assets - Parse array of assets response" << std::endl;

    json arr = json::array();
    json a1 = make_asset_json();
    json a2 = make_asset_json();
    a2["symbol"] = "MSFT";
    a2["name"] = "Microsoft Corporation";
    a2["id"] = "some-other-id";
    arr.push_back(a1);
    arr.push_back(a2);

    std::vector<Asset> assets;
    for (const auto& a : arr) {
        assets.push_back(Asset(a));
    }

    std::cout << "Response: " << assets.size() << " assets" << std::endl;
    for (const auto& a : assets) {
        std::cout << "  " << a.symbol << " (" << a.name << ") - " << a.exchange << std::endl;
    }

    ASSERT_EQ(assets.size(), 2u);
    EXPECT_EQ(assets[0].symbol, "AAPL");
    EXPECT_EQ(assets[1].symbol, "MSFT");

    std::cout << "PASSED" << std::endl;
}
