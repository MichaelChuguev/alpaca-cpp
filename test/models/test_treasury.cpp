#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/treasury.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

namespace {

json make_treasury_json() {
    json j;
    j["cusip"] = "912796YZ3";
    j["isin"] = "US912796YZ30";
    j["bond_status"] = "outstanding";
    j["tradeable"] = true;
    j["subtype"] = "bond";
    j["issue_date"] = "2023-06-15";
    j["maturity_date"] = "2053-06-15";
    j["description"] = "US Treasury Bond 4.375% 2053";
    j["description_short"] = "T 4.375 06/15/53";
    j["close_price"] = 99.5;
    j["close_price_date"] = "2024-01-15";
    j["close_yield_to_maturity"] = 4.42;
    j["close_yield_to_worst"] = 4.42;
    j["coupon"] = 4.375;
    j["coupon_type"] = "fixed";
    j["coupon_frequency"] = "semi_annual";
    j["first_coupon_date"] = "2023-12-15";
    j["next_coupon_date"] = "2024-06-15";
    j["last_coupon_date"] = "2053-06-15";
    return j;
}

} // namespace

TEST(TreasuryTest, ParseFromJson) {
    auto j = make_treasury_json();
    Treasury treasury(j);

    EXPECT_EQ(treasury.cusip, "912796YZ3");
    EXPECT_EQ(treasury.isin, "US912796YZ30");
    EXPECT_EQ(treasury.bond_status, BondStatus::OUTSTANDING);
    EXPECT_TRUE(treasury.tradeable);
    EXPECT_EQ(treasury.subtype, Subtype::BOND);
    EXPECT_EQ(treasury.description, "US Treasury Bond 4.375% 2053");
    EXPECT_EQ(treasury.description_short, "T 4.375 06/15/53");
    EXPECT_EQ(treasury.close_price, Decimal(99.5));
    EXPECT_EQ(treasury.close_yield_to_maturity, Decimal(4.42));
    EXPECT_EQ(treasury.close_yield_to_worst, Decimal(4.42));
    EXPECT_EQ(treasury.coupon, Decimal(4.375));
    EXPECT_EQ(treasury.coupon_type, CouponType::FIXED);
    EXPECT_EQ(treasury.coupon_frequency, CouponFrequency::SEMI_ANNUAL);
}

TEST(TreasuryTest, BillSubtype) {
    auto j = make_treasury_json();
    j["subtype"] = "bill";
    j["coupon_type"] = "zero_coupon";
    j["coupon_frequency"] = "zero";
    j["coupon"] = 0.0;
    Treasury treasury(j);

    EXPECT_EQ(treasury.subtype, Subtype::BILL);
    EXPECT_EQ(treasury.coupon_type, CouponType::ZERO_COUPON);
    EXPECT_EQ(treasury.coupon_frequency, CouponFrequency::ZERO);
}

TEST(TreasuryTest, NoteSubtype) {
    auto j = make_treasury_json();
    j["subtype"] = "note";
    Treasury treasury(j);

    EXPECT_EQ(treasury.subtype, Subtype::NOTE);
}

TEST(TreasuryTest, StripsSubtype) {
    auto j = make_treasury_json();
    j["subtype"] = "strips";
    Treasury treasury(j);

    EXPECT_EQ(treasury.subtype, Subtype::STRIPS);
}

TEST(TreasuryTest, TipsSubtype) {
    auto j = make_treasury_json();
    j["subtype"] = "tips";
    Treasury treasury(j);

    EXPECT_EQ(treasury.subtype, Subtype::TIPS);
}

TEST(TreasuryTest, FloatingRateSubtype) {
    auto j = make_treasury_json();
    j["subtype"] = "floating_rate";
    j["coupon_type"] = "floating";
    Treasury treasury(j);

    EXPECT_EQ(treasury.subtype, Subtype::FLOATING_RATE);
    EXPECT_EQ(treasury.coupon_type, CouponType::FLOATING);
}

TEST(TreasuryTest, MaturedBondStatus) {
    auto j = make_treasury_json();
    j["bond_status"] = "matured";
    Treasury treasury(j);

    EXPECT_EQ(treasury.bond_status, BondStatus::MATURED);
}

TEST(TreasuryTest, PreIssuanceBondStatus) {
    auto j = make_treasury_json();
    j["bond_status"] = "pre_issuance";
    Treasury treasury(j);

    EXPECT_EQ(treasury.bond_status, BondStatus::PRE_ISSUANCE);
}

TEST(TreasuryTest, CouponFrequencyVariants) {
    // Annual
    auto j = make_treasury_json();
    j["coupon_frequency"] = "annual";
    Treasury t1(j);
    EXPECT_EQ(t1.coupon_frequency, CouponFrequency::ANNUAL);

    // Quarterly
    j["coupon_frequency"] = "quarterly";
    Treasury t2(j);
    EXPECT_EQ(t2.coupon_frequency, CouponFrequency::QUARTERLY);

    // Monthly
    j["coupon_frequency"] = "monthly";
    Treasury t3(j);
    EXPECT_EQ(t3.coupon_frequency, CouponFrequency::MONTHLY);
}

TEST(TreasuryTest, ToString) {
    auto j = make_treasury_json();
    Treasury treasury(j);
    std::string str = treasury.to_string();

    EXPECT_NE(str.find("Treasury:"), std::string::npos);
    EXPECT_NE(str.find("912796YZ3"), std::string::npos);
    EXPECT_NE(str.find("outstanding"), std::string::npos);
    EXPECT_NE(str.find("bond"), std::string::npos);
}

TEST(TreasuryTest, ToJson) {
    auto j = make_treasury_json();
    Treasury treasury(j);
    json result = treasury.to_json();

    EXPECT_EQ(result["cusip"], "912796YZ3");
    EXPECT_EQ(result["isin"], "US912796YZ30");
    EXPECT_EQ(result["bond_status"], "outstanding");
    EXPECT_TRUE(result["tradeable"].get<bool>());
    EXPECT_EQ(result["subtype"], "bond");
    EXPECT_EQ(result["coupon_type"], "fixed");
    EXPECT_EQ(result["coupon_frequency"], "semi_annual");
    EXPECT_DOUBLE_EQ(result["coupon"].get<double>(), 4.375);
}

TEST(TreasuryTest, ToJsonRoundTrip) {
    auto j = make_treasury_json();
    Treasury treasury(j);
    json result = treasury.to_json();

    // Parse back
    Treasury treasury2(result);
    EXPECT_EQ(treasury2.cusip, treasury.cusip);
    EXPECT_EQ(treasury2.isin, treasury.isin);
    EXPECT_EQ(treasury2.bond_status, treasury.bond_status);
    EXPECT_EQ(treasury2.subtype, treasury.subtype);
    EXPECT_EQ(treasury2.coupon, treasury.coupon);
}

TEST(TreasuryTest, NotTradeable) {
    auto j = make_treasury_json();
    j["tradeable"] = false;
    Treasury treasury(j);

    EXPECT_FALSE(treasury.tradeable);
}
