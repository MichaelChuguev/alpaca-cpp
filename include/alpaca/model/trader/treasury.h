#pragma once

#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

enum BondStatus {
    OUTSTANDING,
    MATURED,
    PRE_ISSUANCE
};

enum Subtype {
    BOND,
    BILL,
    NOTE,
    STRIPS,
    TIPS,
    FLOATING_RATE
};

enum CouponType {
    FIXED,
    FLOATING,
    ZERO_COUPON
};

enum CouponFrequency {
    ANNUAL,
    SEMI_ANNUAL,
    QUARTERLY,
    MONTHLY,
    ZERO
};

struct Treasury {

    std::string cusip;
    std::string isin;
    BondStatus bond_status;
    bool tradeable;
    Subtype subtype;
    DateTime issue_date;
    DateTime maturity_date;
    std::string description;
    std::string description_short;
    double close_price;
    DateTime close_price_date;
    double close_yield_to_maturity;
    double close_yield_to_worst;
    double coupon;
    CouponType coupon_type;
    CouponFrequency coupon_frequency;
    DateTime first_coupon_date;
    DateTime next_coupon_date;
    DateTime last_coupon_date;

    Treasury(const json& j)
    :
        cusip(j["cusip"].get<std::string>()),
        isin(j["isin"].get<std::string>()),
        bond_status(j["bond_status"].get<std::string>() == "outstanding" ? BondStatus::OUTSTANDING : (j["bond_status"].get<std::string>() == "matured" ? BondStatus::MATURED : BondStatus::PRE_ISSUANCE)),
        tradeable(j["tradeable"].get<bool>()),
        subtype(j["subtype"].get<std::string>() == "bond" ? Subtype::BOND : (j["subtype"].get<std::string>() == "bill" ? Subtype::BILL : (j["subtype"].get<std::string>() == "note" ? Subtype::NOTE : (j["subtype"].get<std::string>() == "strips" ? Subtype::STRIPS : (j["subtype"].get<std::string>() == "tips" ? Subtype::TIPS : Subtype::FLOATING_RATE))))),
        issue_date(DateTime::parse(j["issue_date"].get<std::string>())),
        maturity_date(DateTime::parse(j["maturity_date"].get<std::string>())),
        description(j["description"].get<std::string>()),
        description_short(j["description_short"].get<std::string>()),
        close_price(j["close_price"].get<double>()),
        close_price_date(DateTime::parse(j["close_price_date"].get<std::string>())),
        close_yield_to_maturity(j["close_yield_to_maturity"].get<double>()),
        close_yield_to_worst(j["close_yield_to_worst"].get<double>()),
        coupon(j["coupon"].get<double>()),
        coupon_type(j["coupon_type"].get<std::string>() == "fixed" ? CouponType::FIXED : (j["coupon_type"].get<std::string>() == "floating" ? CouponType::FLOATING : CouponType::ZERO_COUPON)),
        coupon_frequency(j["coupon_frequency"].get<std::string>() == "annual" ? CouponFrequency::ANNUAL : (j["coupon_frequency"].get<std::string>() == "semi_annual" ? CouponFrequency::SEMI_ANNUAL : (j["coupon_frequency"].get<std::string>() == "quarterly" ? CouponFrequency::QUARTERLY : (j["coupon_frequency"].get<std::string>() == "monthly" ? CouponFrequency::MONTHLY : CouponFrequency::ZERO)))),
        first_coupon_date(DateTime::parse(j["first_coupon_date"].get<std::string>())),
        next_coupon_date(DateTime::parse(j["next_coupon_date"].get<std::string>())),
        last_coupon_date(DateTime::parse(j["last_coupon_date"].get<std::string>()))
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Treasury:\n"
            << "CUSIP: " << cusip << "\n"
            << "ISIN: " << isin << "\n"
            << "Bond Status: " << (bond_status == OUTSTANDING ? "OUTSTANDING" : (bond_status == MATURED ? "MATURED" : "PRE_ISSUANCE")) << "\n"
            << "Tradeable: " << tradeable << "\n"
            << "Subtype: " << (subtype == BOND ? "BOND" : (subtype == BILL ? "BILL" : (subtype == NOTE ? "NOTE" : (subtype == STRIPS ? "STRIPS" : (subtype == TIPS ? "TIPS" : "FLOATING"))))) << "\n"
            << "Issue Date: " << issue_date.to_string() << "\n"
            << "Maturity Date: " << maturity_date.to_string() << "\n"
            << "Description: " << description << "\n"
            << "Description Short: " << description_short << "\n"
            << "Close Price: " << close_price << "\n"
            << "Close Price Date: " << close_price_date.to_string() << "\n"
            << "Close Yield to Maturity: " << close_yield_to_maturity << "\n"
            << "Close Yield to Worst: " << close_yield_to_worst << "\n"
            << "Coupon: " << coupon << "\n"
            << "Coupon Type: " << (coupon_type == FIXED ? "FIXED" : (coupon_type == FLOATING ? "FLOATING" : "ZERO"))<< "\n"
            << "Coupon Frequency: "<< (coupon_frequency == ANNUAL ? "ANNUAL" : (coupon_frequency == SEMI_ANNUAL ? "SEMI_ANNUAL" : (coupon_frequency == QUARTERLY ? "QUARTERLY" : (coupon_frequency == MONTHLY ? "MONTHLY" : "ZERO"))))<< "\n"
            <<  "First Coupon Date: " << first_coupon_date.to_string() << "\n"
            <<  "Next Coupon Date: " << next_coupon_date.to_string() << "\n"
            <<  "Last Coupon Date: " << last_coupon_date.to_string() << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["cusip"] = cusip;
        j["isin"] = isin;
        j["bond_status"] = (bond_status == OUTSTANDING ? "outstanding" : (bond_status == MATURED ? "matured" : "pre_issuance"));
        j["tradeable"] = tradeable;
        j["subtype"] = (subtype == BOND ? "bond" : (subtype == BILL ? "bill" : (subtype == NOTE ? "note" : (subtype == STRIPS ? "strips" : (subtype == TIPS ? "tips" : "floating")))));
        j["issue_date"] = issue_date.to_string();
        j["maturity_date"] = maturity_date.to_string();
        j["description"] = description;
        j["description_short"] = description_short;
        j["close_price"] = close_price;
        j["close_price_date"] = close_price_date.to_string();
        j["close_yield_to_maturity"] = close_yield_to_maturity;
        j["close_yield_to_worst"] = close_yield_to_worst;
        j["coupon"] = coupon;
        j["coupon_type"] = (coupon_type == FIXED ? "fixed" : (coupon_type == FLOATING ? "floating" : "zero"));
        j["coupon_frequency"] = (coupon_frequency == ANNUAL ? "annual" : (coupon_frequency == SEMI_ANNUAL ? "semi_annual" : (coupon_frequency == QUARTERLY ? "quarterly" : (coupon_frequency == MONTHLY ? "monthly" : "zero"))));
        j["first_coupon_date"] = first_coupon_date.to_string();
        j["next_coupon_date"] = next_coupon_date.to_string();
        j["last_coupon_date"] = last_coupon_date.to_string();
        return j;
    }

};

} // namespace alpaca