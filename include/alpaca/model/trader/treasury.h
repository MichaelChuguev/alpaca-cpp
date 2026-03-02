#pragma once

#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

enum class BondStatus {
    OUTSTANDING,
    MATURED,
    PRE_ISSUANCE
};

inline std::string bond_status_to_string(BondStatus s) {
    switch (s) {
        case BondStatus::OUTSTANDING:   return "outstanding";
        case BondStatus::MATURED:       return "matured";
        case BondStatus::PRE_ISSUANCE:  return "pre_issuance";
        default: return "";
    }
}

inline BondStatus string_to_bond_status(const std::string& s) {
    if (s == "outstanding")  return BondStatus::OUTSTANDING;
    if (s == "matured")      return BondStatus::MATURED;
    if (s == "pre_issuance") return BondStatus::PRE_ISSUANCE;
    return BondStatus::OUTSTANDING;
}

enum class Subtype {
    BOND,
    BILL,
    NOTE,
    STRIPS,
    TIPS,
    FLOATING_RATE
};

inline std::string subtype_to_string(Subtype s) {
    switch (s) {
        case Subtype::BOND:          return "bond";
        case Subtype::BILL:          return "bill";
        case Subtype::NOTE:          return "note";
        case Subtype::STRIPS:        return "strips";
        case Subtype::TIPS:          return "tips";
        case Subtype::FLOATING_RATE: return "floating_rate";
        default: return "";
    }
}

inline Subtype string_to_subtype(const std::string& s) {
    if (s == "bond")          return Subtype::BOND;
    if (s == "bill")          return Subtype::BILL;
    if (s == "note")          return Subtype::NOTE;
    if (s == "strips")        return Subtype::STRIPS;
    if (s == "tips")          return Subtype::TIPS;
    if (s == "floating_rate") return Subtype::FLOATING_RATE;
    return Subtype::BOND;
}

enum class CouponType {
    FIXED,
    FLOATING,
    ZERO_COUPON
};

inline std::string coupon_type_to_string(CouponType t) {
    switch (t) {
        case CouponType::FIXED:       return "fixed";
        case CouponType::FLOATING:    return "floating";
        case CouponType::ZERO_COUPON: return "zero_coupon";
        default: return "";
    }
}

inline CouponType string_to_coupon_type(const std::string& s) {
    if (s == "fixed")       return CouponType::FIXED;
    if (s == "floating")    return CouponType::FLOATING;
    if (s == "zero_coupon") return CouponType::ZERO_COUPON;
    return CouponType::FIXED;
}

enum class CouponFrequency {
    ANNUAL,
    SEMI_ANNUAL,
    QUARTERLY,
    MONTHLY,
    ZERO
};

inline std::string coupon_frequency_to_string(CouponFrequency f) {
    switch (f) {
        case CouponFrequency::ANNUAL:      return "annual";
        case CouponFrequency::SEMI_ANNUAL: return "semi_annual";
        case CouponFrequency::QUARTERLY:   return "quarterly";
        case CouponFrequency::MONTHLY:     return "monthly";
        case CouponFrequency::ZERO:        return "zero";
        default: return "";
    }
}

inline CouponFrequency string_to_coupon_frequency(const std::string& s) {
    if (s == "annual")      return CouponFrequency::ANNUAL;
    if (s == "semi_annual") return CouponFrequency::SEMI_ANNUAL;
    if (s == "quarterly")   return CouponFrequency::QUARTERLY;
    if (s == "monthly")     return CouponFrequency::MONTHLY;
    if (s == "zero")        return CouponFrequency::ZERO;
    return CouponFrequency::ANNUAL;
}

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
    Decimal close_price;
    DateTime close_price_date;
    Decimal close_yield_to_maturity;
    Decimal close_yield_to_worst;
    Decimal coupon;
    CouponType coupon_type;
    CouponFrequency coupon_frequency;
    DateTime first_coupon_date;
    DateTime next_coupon_date;
    DateTime last_coupon_date;

    Treasury(const json& j)
    :
        cusip(j["cusip"].get<std::string>()),
        isin(j["isin"].get<std::string>()),
        bond_status(string_to_bond_status(j["bond_status"].get<std::string>())),
        tradeable(j["tradeable"].get<bool>()),
        subtype(string_to_subtype(j["subtype"].get<std::string>())),
        issue_date(DateTime::parse(j["issue_date"].get<std::string>())),
        maturity_date(DateTime::parse(j["maturity_date"].get<std::string>())),
        description(j["description"].get<std::string>()),
        description_short(j["description_short"].get<std::string>()),
        close_price(Decimal(j["close_price"].get<double>())),
        close_price_date(DateTime::parse(j["close_price_date"].get<std::string>())),
        close_yield_to_maturity(Decimal(j["close_yield_to_maturity"].get<double>())),
        close_yield_to_worst(Decimal(j["close_yield_to_worst"].get<double>())),
        coupon(Decimal(j["coupon"].get<double>())),
        coupon_type(string_to_coupon_type(j["coupon_type"].get<std::string>())),
        coupon_frequency(string_to_coupon_frequency(j["coupon_frequency"].get<std::string>())),
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
            << "Bond Status: " << bond_status_to_string(bond_status) << "\n"
            << "Tradeable: " << tradeable << "\n"
            << "Subtype: " << subtype_to_string(subtype) << "\n"
            << "Issue Date: " << issue_date.to_string() << "\n"
            << "Maturity Date: " << maturity_date.to_string() << "\n"
            << "Description: " << description << "\n"
            << "Description Short: " << description_short << "\n"
            << "Close Price: " << close_price << "\n"
            << "Close Price Date: " << close_price_date.to_string() << "\n"
            << "Close Yield to Maturity: " << close_yield_to_maturity << "\n"
            << "Close Yield to Worst: " << close_yield_to_worst << "\n"
            << "Coupon: " << coupon << "\n"
            << "Coupon Type: " << coupon_type_to_string(coupon_type) << "\n"
            << "Coupon Frequency: " << coupon_frequency_to_string(coupon_frequency) << "\n"
            <<  "First Coupon Date: " << first_coupon_date.to_string() << "\n"
            <<  "Next Coupon Date: " << next_coupon_date.to_string() << "\n"
            <<  "Last Coupon Date: " << last_coupon_date.to_string() << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["cusip"] = cusip;
        j["isin"] = isin;
        j["bond_status"] = bond_status_to_string(bond_status);
        j["tradeable"] = tradeable;
        j["subtype"] = subtype_to_string(subtype);
        j["issue_date"] = issue_date.to_string();
        j["maturity_date"] = maturity_date.to_string();
        j["description"] = description;
        j["description_short"] = description_short;
        j["close_price"] = close_price.to_double();
        j["close_price_date"] = close_price_date.to_string();
        j["close_yield_to_maturity"] = close_yield_to_maturity.to_double();
        j["close_yield_to_worst"] = close_yield_to_worst.to_double();
        j["coupon"] = coupon.to_double();
        j["coupon_type"] = coupon_type_to_string(coupon_type);
        j["coupon_frequency"] = coupon_frequency_to_string(coupon_frequency);
        j["first_coupon_date"] = first_coupon_date.to_string();
        j["next_coupon_date"] = next_coupon_date.to_string();
        j["last_coupon_date"] = last_coupon_date.to_string();
        return j;
    }

};

} // namespace alpaca