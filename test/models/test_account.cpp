#include <gtest/gtest.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/account.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_account_json() {
    json j;
    j["id"] = "904837e3-3b76-47ec-b432-046db621571b";
    j["account_number"] = "010203ABCD";
    j["status"] = "ACTIVE";
    j["crypto_status"] = "ACTIVE";
    j["options_approved_level"] = 2;
    j["options_trading_level"] = 2;
    j["currency"] = "USD";
    j["buying_power"] = "100000.00";
    j["regt_buying_power"] = "100000.00";
    j["daytrading_buying_power"] = "400000.00";
    j["effective_buying_power"] = "100000.00";
    j["non_marginable_buying_power"] = "50000.00";
    j["options_buying_power"] = "100000.00";
    j["bod_dtbp"] = "400000.00";
    j["cash"] = "100000.00";
    j["accrued_fees"] = "0.00";
    j["portfolio_value"] = "100000.00";
    j["pattern_day_trader"] = false;
    j["trading_blocked"] = false;
    j["transfers_blocked"] = false;
    j["account_blocked"] = false;
    j["created_at"] = "2022-01-01T00:00:00Z";
    j["trade_suspended_by_user"] = false;
    j["multiplier"] = "4";
    j["shorting_enabled"] = true;
    j["equity"] = "100000.00";
    j["last_equity"] = "99000.00";
    j["long_market_value"] = "50000.00";
    j["short_market_value"] = "0.00";
    j["position_market_value"] = "50000.00";
    j["initial_margin"] = "25000.00";
    j["maintenance_margin"] = "12500.00";
    j["last_maintenance_margin"] = "12000.00";
    j["sma"] = "100000.00";
    j["daytrade_count"] = 0;
    j["balance_asof"] = "100000.00";
    j["crypto_tier"] = 1;
    j["intraday_adjustments"] = "0.00";
    j["pending_reg_taf_fees"] = "0.00";
    return j;
}

} // anonymous namespace

TEST(AccountTest, ParseFromJson) {
    std::cout << "\n=== Test: AccountTest::ParseFromJson ===" << std::endl;
    std::cout << "Testing: GET /v2/account - Parse account response from JSON" << std::endl;

    json j = make_account_json();
    Account account(j);

    std::cout << "Response:" << std::endl;
    std::cout << "  ID: " << account.id << std::endl;
    std::cout << "  Account Number: " << account.account_number << std::endl;
    std::cout << "  Status: " << account.status << std::endl;
    std::cout << "  Crypto Status: " << account.crypto_status << std::endl;
    std::cout << "  Currency: " << account.currency << std::endl;
    std::cout << "  Buying Power: " << account.buying_power << std::endl;
    std::cout << "  Cash: " << account.cash << std::endl;
    std::cout << "  Equity: " << account.equity << std::endl;
    std::cout << "  Portfolio Value: " << account.portfolio_value << std::endl;
    std::cout << "  Pattern Day Trader: " << (account.pattern_day_trader ? "true" : "false") << std::endl;
    std::cout << "  Shorting Enabled: " << (account.shorting_enabled ? "true" : "false") << std::endl;
    std::cout << "  Daytrade Count: " << account.daytrade_count << std::endl;

    EXPECT_EQ(account.id, "904837e3-3b76-47ec-b432-046db621571b");
    EXPECT_EQ(account.account_number, "010203ABCD");
    EXPECT_EQ(account.status, "ACTIVE");
    EXPECT_EQ(account.crypto_status, "ACTIVE");
    EXPECT_EQ(account.options_approved_level, 2);
    EXPECT_EQ(account.options_trading_level, 2);
    EXPECT_EQ(account.currency, "USD");
    EXPECT_EQ(account.buying_power, Decimal(100000.00));
    EXPECT_EQ(account.regt_buying_power, Decimal(100000.00));
    EXPECT_EQ(account.daytrading_buying_power, Decimal(400000.00));
    EXPECT_EQ(account.effective_buying_power, Decimal(100000.00));
    EXPECT_EQ(account.non_marginable_buying_power, Decimal(50000.00));
    EXPECT_EQ(account.options_buying_power, Decimal(100000.00));
    EXPECT_EQ(account.bod_dtbp, Decimal(400000.00));
    EXPECT_EQ(account.cash, Decimal(100000.00));
    EXPECT_EQ(account.accrued_fees, Decimal(0.00));
    EXPECT_EQ(account.portfolio_value, Decimal(100000.00));
    EXPECT_FALSE(account.pattern_day_trader);
    EXPECT_FALSE(account.trading_blocked);
    EXPECT_FALSE(account.transfers_blocked);
    EXPECT_FALSE(account.account_blocked);
    EXPECT_FALSE(account.trade_suspended_by_user);
    EXPECT_EQ(account.multiplier, "4");
    EXPECT_TRUE(account.shorting_enabled);
    EXPECT_EQ(account.equity, Decimal(100000.00));
    EXPECT_EQ(account.last_equity, Decimal(99000.00));
    EXPECT_EQ(account.long_market_value, Decimal(50000.00));
    EXPECT_EQ(account.short_market_value, Decimal(0.00));
    EXPECT_EQ(account.position_market_value, Decimal(50000.00));
    EXPECT_EQ(account.initial_margin, Decimal(25000.00));
    EXPECT_EQ(account.maintenance_margin, Decimal(12500.00));
    EXPECT_EQ(account.last_maintenance_margin, Decimal(12000.00));
    EXPECT_EQ(account.sma, Decimal(100000.00));
    EXPECT_EQ(account.daytrade_count, 0);
    EXPECT_EQ(account.balance_asof, Decimal(100000.00));
    EXPECT_EQ(account.crypto_tier, 1);
    EXPECT_EQ(account.intraday_adjustments, Decimal(0.00));
    EXPECT_EQ(account.pending_reg_taf_fees, Decimal(0.00));

    std::cout << "PASSED" << std::endl;
}

TEST(AccountTest, ToJson) {
    std::cout << "\n=== Test: AccountTest::ToJson ===" << std::endl;
    std::cout << "Testing: Account model to_json() serialization" << std::endl;

    json j = make_account_json();
    Account account(j);
    json result = account.to_json();

    std::cout << "Response JSON (partial):" << std::endl;
    std::cout << "  id: " << result["id"] << std::endl;
    std::cout << "  account_number: " << result["account_number"] << std::endl;
    std::cout << "  status: " << result["status"] << std::endl;
    std::cout << "  currency: " << result["currency"] << std::endl;

    EXPECT_EQ(result["id"], "904837e3-3b76-47ec-b432-046db621571b");
    EXPECT_EQ(result["account_number"], "010203ABCD");
    EXPECT_EQ(result["status"], "ACTIVE");
    EXPECT_EQ(result["currency"], "USD");
    EXPECT_FALSE(result["pattern_day_trader"].get<bool>());
    EXPECT_TRUE(result["shorting_enabled"].get<bool>());

    std::cout << "PASSED" << std::endl;
}

TEST(AccountTest, ToString) {
    std::cout << "\n=== Test: AccountTest::ToString ===" << std::endl;
    std::cout << "Testing: Account model to_string() output" << std::endl;

    json j = make_account_json();
    Account account(j);
    std::string str = account.to_string();

    std::cout << "Response:\n" << str << std::endl;

    EXPECT_NE(str.find("Account Information:"), std::string::npos);
    EXPECT_NE(str.find("010203ABCD"), std::string::npos);
    EXPECT_NE(str.find("ACTIVE"), std::string::npos);

    std::cout << "PASSED" << std::endl;
}
