#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>

#include "nlohmann/json.hpp"
#include "alpaca/AlpacaBrokerAPI.h"
#include "alpaca/model/broker/brokerAccountTradingDetails.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_broker_account_trading_details_json() {
    json j;
    j["id"] = "56712986-9ff7-4d8f-8e52-077e099e533e";
    j["admin_configurations"] = {
        {"outgoing_transfers_blocked", false},
        {"incoming_transfers_blocked", false},
        {"disable_shorting", true},
        {"pdt_check_mode", "entry"},
        {"disable_fractional", false},
        {"disable_crypto", false},
        {"disable_day_trading", false},
        {"allow_instant_ach", true},
        {"disable_algodash_access", false},
        {"disable_api_key", false},
        {"max_margin_multiplier", {{"from", "4"}, {"to", "2"}}},
        {"max_options_trading_level", {{"from", "3"}, {"to", "2"}}},
        {"acct_daily_transfer_limit", "25000"},
        {"restrict_to_liquidation_reasons", {
            {"pattern_day_trading", false},
            {"ach_return", true},
            {"position_to_equity_ratio", false},
            {"unspecified", false}
        }}
    };
    j["user_configurations"] = {
        {"dtbp_check", "both"},
        {"trade_confirm_email", "all"},
        {"suspend_trade", false},
        {"no_shorting", false},
        {"fractional_trading", true},
        {"max_margin_multiplier", "2"},
        {"max_options_trading_level", 2},
        {"pdt_check", "entry"},
        {"ptp_no_exception_entry", "false"},
        {"disable_overnight_trading", true}
    };
    j["account_number"] = "601612064";
    j["status"] = "ACTIVE";
    j["crypto_status"] = "PAPER_ONLY";
    j["currency"] = "USD";
    j["buying_power"] = "83567.42";
    j["regt_buying_power"] = "83567.42";
    j["daytrading_buying_power"] = "0";
    j["effective_buying_power"] = "83567.42";
    j["non_marginable_buying_power"] = "41783.71";
    j["options_buying_power"] = "83567.42";
    j["bod_dtbp"] = "0";
    j["cash"] = "83567.42";
    j["cash_withdrawable"] = "0";
    j["cash_transferable"] = "41783.71";
    j["pending_transfer_out"] = "0";
    j["pending_transfer_in"] = "0";
    j["accrued_fees"] = "0";
    j["portfolio_value"] = "83567.42";
    j["pattern_day_trader"] = false;
    j["trading_blocked"] = false;
    j["transfers_blocked"] = false;
    j["account_blocked"] = false;
    j["created_at"] = "2022-01-21T21:25:26.713802Z";
    j["trade_suspended_by_user"] = false;
    j["multiplier"] = "1";
    j["shorting_enabled"] = false;
    j["equity"] = "83567.42";
    j["last_equity"] = "41783.71";
    j["long_market_value"] = "0";
    j["short_market_value"] = "0";
    j["position_market_value"] = "0";
    j["initial_margin"] = "0";
    j["maintenance_margin"] = "0";
    j["last_maintenance_margin"] = "0";
    j["sma"] = "0";
    j["daytrade_count"] = 0;
    j["balance_asof"] = "2022-02-08";
    j["previous_close"] = "2022-02-08T19:00:00-05:00";
    j["last_long_market_value"] = "0";
    j["last_short_market_value"] = "0";
    j["last_cash"] = "41783.71";
    j["last_initial_margin"] = "0";
    j["last_regt_buying_power"] = "41783.71";
    j["last_daytrading_buying_power"] = "0";
    j["last_options_buying_power"] = "41783.71";
    j["last_buying_power"] = "41783.71";
    j["last_daytrade_count"] = 0;
    j["clearing_broker"] = "VELOX";
    j["options_approved_level"] = 2;
    j["options_trading_level"] = 1;
    j["intraday_adjustments"] = "0";
    j["pending_reg_taf_fees"] = "0.01";
    j["memoposts"] = "100";
    return j;
}

} // namespace

static_assert(
    std::is_same_v<
        decltype(&AlpacaBrokerAPI::get_trading_details_for_account),
        BrokerAccountTradingDetails (AlpacaBrokerAPI::*)(const std::string&)
    >,
    "Broker API trading-details method signature changed unexpectedly");

TEST(BrokerAccountTradingDetailsTest, ParseFromJson) {
    std::cout << "\n=== Test: BrokerAccountTradingDetailsTest::ParseFromJson ===" << std::endl;

    const json j = make_broker_account_trading_details_json();
    const BrokerAccountTradingDetails account(j);

    EXPECT_EQ(account.id, "56712986-9ff7-4d8f-8e52-077e099e533e");
    EXPECT_EQ(account.account_number, "601612064");
    EXPECT_EQ(account.status, "ACTIVE");
    EXPECT_EQ(account.crypto_status, "PAPER_ONLY");
    EXPECT_EQ(account.buying_power, Decimal(83567.42));
    EXPECT_EQ(account.cash_withdrawable, Decimal(0));
    EXPECT_EQ(account.pending_transfer_in, Decimal(0));
    EXPECT_EQ(account.balance_asof.to_iso_string(), "2022-02-08");
    EXPECT_EQ(account.previous_close.to_iso_string(), "2022-02-08T19:00:00-05:00");
    EXPECT_EQ(account.clearing_broker, "VELOX");
    EXPECT_EQ(account.options_approved_level, 2);
    EXPECT_EQ(account.options_trading_level, 1);
    EXPECT_EQ(account.memoposts, Decimal(100));
    EXPECT_EQ(account.user_configurations.max_margin_multiplier, "2");
    EXPECT_TRUE(account.user_configurations.disable_overnight_trading);
    EXPECT_TRUE(account.admin_configurations.disable_shorting);
    EXPECT_EQ(account.admin_configurations.max_margin_multiplier.from, "4");
    EXPECT_EQ(account.admin_configurations.max_margin_multiplier.to, "2");
    EXPECT_TRUE(account.admin_configurations.restrict_to_liquidation_reasons.ach_return);
}

TEST(BrokerAccountTradingDetailsTest, ToJson) {
    std::cout << "\n=== Test: BrokerAccountTradingDetailsTest::ToJson ===" << std::endl;

    const BrokerAccountTradingDetails account(make_broker_account_trading_details_json());
    const json result = account.to_json();

    EXPECT_EQ(result["status"], "ACTIVE");
    EXPECT_EQ(result["crypto_status"], "PAPER_ONLY");
    EXPECT_EQ(result["balance_asof"], "2022-02-08");
    EXPECT_EQ(result["previous_close"], "2022-02-08T19:00:00-05:00");
    EXPECT_EQ(result["clearing_broker"], "VELOX");
    EXPECT_EQ(result["user_configurations"]["disable_overnight_trading"], true);
    EXPECT_EQ(result["admin_configurations"]["max_margin_multiplier"]["from"], "4");
    EXPECT_EQ(result["admin_configurations"]["max_margin_multiplier"]["to"], "2");
}

TEST(BrokerAccountTradingDetailsTest, ToString) {
    std::cout << "\n=== Test: BrokerAccountTradingDetailsTest::ToString ===" << std::endl;

    const BrokerAccountTradingDetails account(make_broker_account_trading_details_json());
    const std::string str = account.to_string();

    EXPECT_NE(str.find("BrokerAccountTradingDetails Information:"), std::string::npos);
    EXPECT_NE(str.find("601612064"), std::string::npos);
    EXPECT_NE(str.find("VELOX"), std::string::npos);
}
