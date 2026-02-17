#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/account.h"

using json = nlohmann::json;
using namespace alpaca;

TEST(AccountConfigTest, ParseFromJson) {
    std::cout << "\n=== Test: AccountConfigTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: GET /v2/account/configurations - Parse config response" << std::endl;

    json j;
    j["dtbp_check"] = "both";
    j["trade_confirm_email"] = "all";
    j["suspend_trade"] = false;
    j["no_shorting"] = false;
    j["fractional_trading"] = true;
    j["max_margin_multiplier"] = "4";
    j["max_options_trading_level"] = 2;
    j["pdt_check"] = "entry";
    j["ptp_no_exception_entry"] = false;

    AccountConfig config(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  DTBP Check: " << config.dtbp_check << std::endl;
    std::cout << "  Trade Confirm Email: " << config.trade_confirm_email << std::endl;
    std::cout << "  Fractional Trading: " << (config.fractional_trading ? "true" : "false") << std::endl;
    std::cout << "  Max Margin Multiplier: " << config.max_margin_multiplier << std::endl;
    std::cout << "  Max Options Trading Level: " << config.max_options_trading_level << std::endl;
    std::cout << "  PDT Check: " << config.pdt_check << std::endl;

    EXPECT_EQ(config.dtbp_check, "both");
    EXPECT_EQ(config.trade_confirm_email, "all");
    EXPECT_FALSE(config.suspend_trade);
    EXPECT_FALSE(config.no_shorting);
    EXPECT_TRUE(config.fractional_trading);
    EXPECT_EQ(config.max_margin_multiplier, 4);
    EXPECT_EQ(config.max_options_trading_level, 2);
    EXPECT_EQ(config.pdt_check, "entry");
    EXPECT_FALSE(config.ptp_no_exception_entry);

    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ConstructWithParams) {
    std::cout << "\n=== Test: AccountConfigTest::ConstructWithParams ===" << std::endl;
    std::cout << "Testing: PATCH /v2/account/configurations - Build config update request" << std::endl;

    AccountConfig config("both", "all", false, false, true, 4, 2, "entry", false);

    std::cout << "Constructed config: dtbp_check=" << config.dtbp_check
              << ", fractional_trading=" << (config.fractional_trading ? "true" : "false") << std::endl;

    EXPECT_EQ(config.dtbp_check, "both");
    EXPECT_EQ(config.trade_confirm_email, "all");
    EXPECT_FALSE(config.suspend_trade);
    EXPECT_FALSE(config.no_shorting);
    EXPECT_TRUE(config.fractional_trading);
    EXPECT_EQ(config.max_margin_multiplier, 4);
    EXPECT_EQ(config.max_options_trading_level, 2);
    EXPECT_EQ(config.pdt_check, "entry");
    EXPECT_FALSE(config.ptp_no_exception_entry);

    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ToJson) {
    std::cout << "\n=== Test: AccountConfigTest::ToJson ===" << std::endl;
    std::cout << "Testing: AccountConfig to_json() serialization for PATCH body" << std::endl;

    AccountConfig config("both", "all", false, false, true, 4, 2, "entry", false);
    json result = config.to_json();

    std::cout << "Serialized JSON: " << result.dump(2) << std::endl;

    EXPECT_EQ(result["dtbp_check"], "both");
    EXPECT_EQ(result["trade_confirm_email"], "all");
    EXPECT_FALSE(result["suspend_trade"].get<bool>());
    EXPECT_EQ(result["max_margin_multiplier"], "4");
    EXPECT_EQ(result["max_options_trading_level"], 2);

    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ToString) {
    std::cout << "\n=== Test: AccountConfigTest::ToString ===" << std::endl;
    std::cout << "Testing: AccountConfig to_string() display" << std::endl;

    AccountConfig config("both", "all", false, false, true, 4, 2, "entry", false);
    std::string str = config.to_string();

    std::cout << "Response:\n" << str << std::endl;

    EXPECT_NE(str.find("Account Configuration:"), std::string::npos);
    EXPECT_NE(str.find("both"), std::string::npos);

    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ValidationFailsInvalidDtbpCheck) {
    std::cout << "\n=== Test: AccountConfigTest::ValidationFailsInvalidDtbpCheck ===" << std::endl;
    std::cout << "Testing: Validation rejects invalid dtbp_check='invalid'" << std::endl;

    AccountConfig config("invalid", "all", false, false, true, 4, 2, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);

    std::cout << "Correctly threw std::invalid_argument" << std::endl;
    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ValidationFailsInvalidTradeConfirmEmail) {
    std::cout << "\n=== Test: AccountConfigTest::ValidationFailsInvalidTradeConfirmEmail ===" << std::endl;
    std::cout << "Testing: Validation rejects invalid trade_confirm_email='invalid'" << std::endl;

    AccountConfig config("both", "invalid", false, false, true, 4, 2, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);

    std::cout << "Correctly threw std::invalid_argument" << std::endl;
    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ValidationFailsInvalidMarginMultiplier) {
    std::cout << "\n=== Test: AccountConfigTest::ValidationFailsInvalidMarginMultiplier ===" << std::endl;
    std::cout << "Testing: Validation rejects invalid max_margin_multiplier=3" << std::endl;

    AccountConfig config("both", "all", false, false, true, 3, 2, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);

    std::cout << "Correctly threw std::invalid_argument" << std::endl;
    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ValidationFailsInvalidOptionsLevel) {
    std::cout << "\n=== Test: AccountConfigTest::ValidationFailsInvalidOptionsLevel ===" << std::endl;
    std::cout << "Testing: Validation rejects invalid max_options_trading_level=5" << std::endl;

    AccountConfig config("both", "all", false, false, true, 4, 5, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);

    std::cout << "Correctly threw std::invalid_argument" << std::endl;
    std::cout << "PASSED" << std::endl;
}

TEST(AccountConfigTest, ValidationFailsInvalidPdtCheck) {
    std::cout << "\n=== Test: AccountConfigTest::ValidationFailsInvalidPdtCheck ===" << std::endl;
    std::cout << "Testing: Validation rejects invalid pdt_check='invalid'" << std::endl;

    AccountConfig config("both", "all", false, false, true, 4, 2, "invalid", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);

    std::cout << "Correctly threw std::invalid_argument" << std::endl;
    std::cout << "PASSED" << std::endl;
}
