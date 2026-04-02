#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>
#include <vector>

#include "nlohmann/json.hpp"
#include "alpaca/AlpacaBrokerAPI.h"
#include "alpaca/model/broker/brokerInstantFunding.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_broker_instant_funding_json() {
    return {
        {"id", "3fa85f64-5717-4562-b3fc-2c963f66afa6"},
        {"amount", "1500.25"},
        {"account_no", "ACC-123"},
        {"source_account_no", "SRC-456"},
        {"status", "PENDING"},
        {"system_date", "2026-03-29"},
        {"deadline", "2026-03-29"},
        {"created_at", "2026-03-29T11:40:28.110Z"},
        {"total_interest", "15.75"},
        {"remaining_payable", "1516.00"},
        {"interests", json::array({
            {
                {"id", "int-3fa85f64-5717-4562-b3fc-2c963f66afa6"},
                {"date", "2026-03-29"},
                {"amount", "5.25"},
                {"status", "PENDING"},
                {"created_at", "2026-03-29T11:40:28.110Z"},
                {"reconciled_at", "2026-03-29T11:40:28.110Z"}
            }
        })},
        {"fees", json::array({
            {
                {"id", "fee-3fa85f64-5717-4562-b3fc-2c963f66afa6"},
                {"amount", "2.50"},
                {"type", "partner"}
            }
        })}
    };
}

} // namespace

static_assert(
    std::is_same_v<
        decltype(&AlpacaBrokerAPI::get_instant_funding_list),
        std::vector<BrokerInstantFunding> (AlpacaBrokerAPI::*)(const std::string&)
    >,
    "Broker API instant-funding-list method signature changed unexpectedly");

TEST(BrokerInstantFundingTest, ParseFromJson) {
    std::cout << "\n=== Test: BrokerInstantFundingTest::ParseFromJson ===" << std::endl;

    const BrokerInstantFunding funding(make_broker_instant_funding_json());

    EXPECT_EQ(funding.id, "3fa85f64-5717-4562-b3fc-2c963f66afa6");
    EXPECT_EQ(funding.amount, Decimal(1500.25));
    EXPECT_EQ(funding.account_no, "ACC-123");
    EXPECT_EQ(funding.source_account_no, "SRC-456");
    EXPECT_EQ(funding.status, "PENDING");
    EXPECT_EQ(funding.system_date.to_iso_string(), "2026-03-29");
    EXPECT_EQ(funding.deadline.to_iso_string(), "2026-03-29");
    EXPECT_EQ(funding.created_at.to_iso_string(), "2026-03-29T11:40:28.110000+00:00");
    EXPECT_EQ(funding.total_interest, Decimal(15.75));
    EXPECT_EQ(funding.remaining_payable, Decimal(1516.0));
    ASSERT_EQ(funding.interests.size(), 1u);
    EXPECT_EQ(funding.interests[0].amount, Decimal(5.25));
    EXPECT_EQ(funding.interests[0].status, "PENDING");
    ASSERT_EQ(funding.fees.size(), 1u);
    EXPECT_EQ(funding.fees[0].amount, Decimal(2.5));
    EXPECT_EQ(funding.fees[0].type, "partner");
}

TEST(BrokerInstantFundingTest, ToJson) {
    std::cout << "\n=== Test: BrokerInstantFundingTest::ToJson ===" << std::endl;

    const BrokerInstantFunding funding(make_broker_instant_funding_json());
    const json result = funding.to_json();

    EXPECT_EQ(result["id"], "3fa85f64-5717-4562-b3fc-2c963f66afa6");
    EXPECT_EQ(result["amount"], "1500.25");
    EXPECT_EQ(result["status"], "PENDING");
    EXPECT_EQ(result["system_date"], "2026-03-29");
    EXPECT_EQ(result["total_interest"], "15.75");
    ASSERT_EQ(result["interests"].size(), 1u);
    EXPECT_EQ(result["interests"][0]["amount"], "5.25");
    EXPECT_EQ(result["interests"][0]["reconciled_at"], "2026-03-29T11:40:28.110000+00:00");
    ASSERT_EQ(result["fees"].size(), 1u);
    EXPECT_EQ(result["fees"][0]["amount"], "2.5");
    EXPECT_EQ(result["fees"][0]["type"], "partner");
}

TEST(BrokerInstantFundingTest, ToString) {
    std::cout << "\n=== Test: BrokerInstantFundingTest::ToString ===" << std::endl;

    const BrokerInstantFunding funding(make_broker_instant_funding_json());
    const std::string str = funding.to_string();

    EXPECT_NE(str.find("ACC-123"), std::string::npos);
    EXPECT_NE(str.find("interests"), std::string::npos);
    EXPECT_NE(str.find("fees"), std::string::npos);
}
