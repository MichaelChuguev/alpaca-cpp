#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/account.h"

using json = nlohmann::json;
using namespace alpaca;

TEST(AccountConfigTest, ParseFromJson) {
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

    EXPECT_EQ(config.dtbp_check, "both");
    EXPECT_EQ(config.trade_confirm_email, "all");
    EXPECT_FALSE(config.suspend_trade);
    EXPECT_FALSE(config.no_shorting);
    EXPECT_TRUE(config.fractional_trading);
    EXPECT_EQ(config.max_margin_multiplier, 4);
    EXPECT_EQ(config.max_options_trading_level, 2);
    EXPECT_EQ(config.pdt_check, "entry");
    EXPECT_FALSE(config.ptp_no_exception_entry);
}

TEST(AccountConfigTest, ConstructWithParams) {
    AccountConfig config("both", "all", false, false, true, 4, 2, "entry", false);

    EXPECT_EQ(config.dtbp_check, "both");
    EXPECT_EQ(config.trade_confirm_email, "all");
    EXPECT_FALSE(config.suspend_trade);
    EXPECT_FALSE(config.no_shorting);
    EXPECT_TRUE(config.fractional_trading);
    EXPECT_EQ(config.max_margin_multiplier, 4);
    EXPECT_EQ(config.max_options_trading_level, 2);
    EXPECT_EQ(config.pdt_check, "entry");
    EXPECT_FALSE(config.ptp_no_exception_entry);
}

TEST(AccountConfigTest, ToJson) {
    AccountConfig config("both", "all", false, false, true, 4, 2, "entry", false);
    json result = config.to_json();

    EXPECT_EQ(result["dtbp_check"], "both");
    EXPECT_EQ(result["trade_confirm_email"], "all");
    EXPECT_FALSE(result["suspend_trade"].get<bool>());
    EXPECT_EQ(result["max_margin_multiplier"], "4");
    EXPECT_EQ(result["max_options_trading_level"], 2);
}

TEST(AccountConfigTest, ToString) {
    AccountConfig config("both", "all", false, false, true, 4, 2, "entry", false);
    std::string str = config.to_string();

    EXPECT_NE(str.find("Account Configuration:"), std::string::npos);
    EXPECT_NE(str.find("both"), std::string::npos);
}

TEST(AccountConfigTest, ValidationFailsInvalidDtbpCheck) {
    AccountConfig config("invalid", "all", false, false, true, 4, 2, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);
}

TEST(AccountConfigTest, ValidationFailsInvalidTradeConfirmEmail) {
    AccountConfig config("both", "invalid", false, false, true, 4, 2, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);
}

TEST(AccountConfigTest, ValidationFailsInvalidMarginMultiplier) {
    AccountConfig config("both", "all", false, false, true, 3, 2, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);
}

TEST(AccountConfigTest, ValidationFailsInvalidOptionsLevel) {
    AccountConfig config("both", "all", false, false, true, 4, 5, "entry", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);
}

TEST(AccountConfigTest, ValidationFailsInvalidPdtCheck) {
    AccountConfig config("both", "all", false, false, true, 4, 2, "invalid", false);
    EXPECT_THROW(config.to_json(), std::invalid_argument);
}
