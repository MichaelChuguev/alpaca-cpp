#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/option.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_deliverable_json() {
    json j;
    j["type"] = "equity";
    j["symbol"] = "AAPL";
    j["asset_id"] = "asset-123";
    j["amount"] = "100";
    j["allocation_percentage"] = "100.0";
    j["settlement_type"] = "T+1";
    j["settlement_method"] = "cash";
    j["delayed_settlement"] = false;
    return j;
}

json make_option_contract_json(bool with_deliverables = true) {
    json j;
    j["id"] = "opt-123";
    j["symbol"] = "AAPL240119C00150000";
    j["name"] = "AAPL Jan 19 2024 150 Call";
    j["status"] = "active";
    j["tradable"] = true;
    j["expiration_date"] = "2024-01-19";
    j["root_symbol"] = "AAPL";
    j["underlying_symbol"] = "AAPL";
    j["underlying_asset_id"] = "asset-aapl";
    j["type"] = "call";
    j["style"] = "american";
    j["strike_price"] = "150.00";
    j["multiplier"] = "100";
    j["size"] = "100";
    j["open_interest"] = "5000";
    j["open_interest_date"] = "2024-01-15";
    j["close_price"] = "5.25";
    j["close_price_date"] = "2024-01-15";

    if (with_deliverables) {
        j["deliverables"] = json::array();
        j["deliverables"].push_back(make_deliverable_json());
    }

    return j;
}

} // namespace

// ===== Deliverable Tests =====

TEST(DeliverableTest, ParseFromJson) {
    auto j = make_deliverable_json();
    OptionContract::Deliverable deliv(j);

    EXPECT_EQ(deliv.type, "equity");
    EXPECT_EQ(deliv.symbol, "AAPL");
    EXPECT_EQ(deliv.asset_id, "asset-123");
    EXPECT_EQ(deliv.amount, Decimal(100.0));
    EXPECT_EQ(deliv.allocation_percentage, Decimal(100.0));
    EXPECT_EQ(deliv.settlement_type, "T+1");
    EXPECT_EQ(deliv.settlement_method, "cash");
    EXPECT_FALSE(deliv.delayed_settlement);
}

TEST(DeliverableTest, ToString) {
    auto j = make_deliverable_json();
    OptionContract::Deliverable deliv(j);
    std::string str = deliv.to_string();

    EXPECT_NE(str.find("Deliverable:"), std::string::npos);
    EXPECT_NE(str.find("AAPL"), std::string::npos);
}

TEST(DeliverableTest, ToJson) {
    auto j = make_deliverable_json();
    OptionContract::Deliverable deliv(j);
    json result = deliv.to_json();

    EXPECT_EQ(result["type"], "equity");
    EXPECT_EQ(result["symbol"], "AAPL");
    EXPECT_FALSE(result["delayed_settlement"].get<bool>());
}

// ===== OptionContract Tests =====

TEST(OptionContractTest, ParseFromJson) {
    auto j = make_option_contract_json();
    OptionContract opt(j);

    EXPECT_EQ(opt.id, "opt-123");
    EXPECT_EQ(opt.symbol, "AAPL240119C00150000");
    EXPECT_EQ(opt.name, "AAPL Jan 19 2024 150 Call");
    EXPECT_EQ(opt.status, "active");
    EXPECT_TRUE(opt.tradable);
    EXPECT_EQ(opt.root_symbol, "AAPL");
    EXPECT_EQ(opt.underlying_symbol, "AAPL");
    EXPECT_EQ(opt.underlying_asset_id, "asset-aapl");
    EXPECT_EQ(opt.type, "call");
    EXPECT_EQ(opt.style, "american");
    EXPECT_EQ(opt.strike_price, Decimal(150.0));
    EXPECT_EQ(opt.multiplier, Decimal(100.0));
    EXPECT_EQ(opt.size, Decimal(100.0));
    EXPECT_EQ(opt.open_interest, Decimal(5000.0));
    EXPECT_EQ(opt.close_price, Decimal(5.25));
    ASSERT_EQ(opt.deliverables.size(), 1u);
    EXPECT_EQ(opt.deliverables[0].symbol, "AAPL");
}

TEST(OptionContractTest, ParseWithoutDeliverables) {
    auto j = make_option_contract_json(false);
    OptionContract opt(j);

    EXPECT_EQ(opt.id, "opt-123");
    EXPECT_TRUE(opt.deliverables.empty());
}

TEST(OptionContractTest, ParseWithMissingOptionalFields) {
    json j;
    j["id"] = "opt-minimal";
    j["symbol"] = "SPY240119P00400000";

    OptionContract opt(j);

    EXPECT_EQ(opt.id, "opt-minimal");
    EXPECT_EQ(opt.symbol, "SPY240119P00400000");
    EXPECT_EQ(opt.name, "");
    EXPECT_FALSE(opt.tradable);
    EXPECT_EQ(opt.strike_price, Decimal(0.0));
}

TEST(OptionContractTest, ToString) {
    auto j = make_option_contract_json();
    OptionContract opt(j);
    std::string str = opt.to_string();

    EXPECT_NE(str.find("Option Contract:"), std::string::npos);
    EXPECT_NE(str.find("AAPL240119C00150000"), std::string::npos);
    EXPECT_NE(str.find("call"), std::string::npos);
    EXPECT_NE(str.find("american"), std::string::npos);
}

TEST(OptionContractTest, ToJson) {
    auto j = make_option_contract_json();
    OptionContract opt(j);
    json result = opt.to_json();

    EXPECT_EQ(result["id"], "opt-123");
    EXPECT_EQ(result["symbol"], "AAPL240119C00150000");
    EXPECT_EQ(result["type"], "call");
    EXPECT_EQ(result["style"], "american");
    EXPECT_TRUE(result["tradable"].get<bool>());
    EXPECT_TRUE(result.contains("deliverables"));
    EXPECT_EQ(result["deliverables"].size(), 1u);
}

TEST(OptionContractTest, PutType) {
    auto j = make_option_contract_json();
    j["type"] = "put";
    j["style"] = "european";
    OptionContract opt(j);

    EXPECT_EQ(opt.type, "put");
    EXPECT_EQ(opt.style, "european");
}

TEST(OptionContractTest, NextPageToken) {
    auto j = make_option_contract_json();
    j["next_page_token"] = "token-abc";
    OptionContract opt(j);

    EXPECT_EQ(opt.next_page_token, "token-abc");
}
