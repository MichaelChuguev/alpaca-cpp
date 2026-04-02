#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

struct BrokerInstantFundingInterest {
    std::string id;
    DateTime date;
    Decimal amount;
    std::string status;
    DateTime created_at;
    DateTime reconciled_at;

    BrokerInstantFundingInterest() = default;
    explicit BrokerInstantFundingInterest(const json& j)
        : id(parse_string(j, "id")),
          date(parse_datetime(j, "date")),
          amount(parse_decimal(j, "amount")),
          status(parse_string(j, "status")),
          created_at(parse_datetime(j, "created_at")),
          reconciled_at(parse_datetime(j, "reconciled_at")) {}

    json to_json() const {
        json j;
        j["id"] = id;
        j["date"] = date.to_iso_string();
        j["amount"] = amount.to_string();
        j["status"] = status;
        j["created_at"] = created_at.to_iso_string();
        j["reconciled_at"] = reconciled_at.to_iso_string();
        return j;
    }
};

struct BrokerInstantFundingFee {
    std::string id;
    Decimal amount;
    std::string type;

    BrokerInstantFundingFee() = default;
    explicit BrokerInstantFundingFee(const json& j)
        : id(parse_string(j, "id")),
          amount(parse_decimal(j, "amount")),
          type(parse_string(j, "type")) {}

    json to_json() const {
        json j;
        j["id"] = id;
        j["amount"] = amount.to_string();
        j["type"] = type;
        return j;
    }
};

struct BrokerInstantFunding {
    std::string id;
    Decimal amount;
    std::string account_no;
    std::string source_account_no;
    std::string status;
    DateTime system_date;
    DateTime deadline;
    DateTime created_at;
    Decimal total_interest;
    Decimal remaining_payable;
    std::vector<BrokerInstantFundingInterest> interests;
    std::vector<BrokerInstantFundingFee> fees;

    BrokerInstantFunding() = default;
    explicit BrokerInstantFunding(const json& j)
        : id(parse_string(j, "id")),
          amount(parse_decimal(j, "amount")),
          account_no(parse_string(j, "account_no")),
          source_account_no(parse_string(j, "source_account_no")),
          status(parse_string(j, "status")),
          system_date(parse_datetime(j, "system_date")),
          deadline(parse_datetime(j, "deadline")),
          created_at(parse_datetime(j, "created_at")),
          total_interest(parse_decimal(j, "total_interest")),
          remaining_payable(parse_decimal(j, "remaining_payable")) {
        if (j.contains("interests") && j["interests"].is_array()) {
            interests.reserve(j["interests"].size());
            for (const auto& item : j["interests"]) {
                interests.emplace_back(item);
            }
        }
        if (j.contains("fees") && j["fees"].is_array()) {
            fees.reserve(j["fees"].size());
            for (const auto& item : j["fees"]) {
                fees.emplace_back(item);
            }
        }
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["amount"] = amount.to_string();
        j["account_no"] = account_no;
        j["source_account_no"] = source_account_no;
        j["status"] = status;
        j["system_date"] = system_date.to_iso_string();
        j["deadline"] = deadline.to_iso_string();
        j["created_at"] = created_at.to_iso_string();
        j["total_interest"] = total_interest.to_string();
        j["remaining_payable"] = remaining_payable.to_string();
        j["interests"] = json::array();
        for (const auto& interest : interests) {
            j["interests"].push_back(interest.to_json());
        }
        j["fees"] = json::array();
        for (const auto& fee : fees) {
            j["fees"].push_back(fee.to_json());
        }
        return j;
    }

    std::string to_string() const {
        return to_json().dump(2);
    }
};

} // namespace alpaca
