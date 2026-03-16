#pragma once

#include <string>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

enum class OptionsApprovalRequester { CORRESPONDENT, ALPACA_ADMIN };

inline OptionsApprovalStatus options_approval_status_from_string(const std::string& s) {
    if (s == "PENDING")              return OptionsApprovalStatus::PENDING;
    if (s == "APPROVED")             return OptionsApprovalStatus::APPROVED;
    if (s == "LOWER_LEVEL_APPROVED") return OptionsApprovalStatus::LOWER_LEVEL_APPROVED;
    if (s == "REJECTED")             return OptionsApprovalStatus::REJECTED;
    return OptionsApprovalStatus::UNSET;
}

inline std::string options_approval_status_to_string(OptionsApprovalStatus s) {
    switch (s) {
        case OptionsApprovalStatus::PENDING:              return "PENDING";
        case OptionsApprovalStatus::APPROVED:             return "APPROVED";
        case OptionsApprovalStatus::LOWER_LEVEL_APPROVED: return "LOWER_LEVEL_APPROVED";
        case OptionsApprovalStatus::REJECTED:             return "REJECTED";
        default:                                          return "";
    }
}

inline OptionsApprovalRequester options_approval_requester_from_string(const std::string& s) {
    if (s == "ALPACA_ADMIN") return OptionsApprovalRequester::ALPACA_ADMIN;
    return OptionsApprovalRequester::CORRESPONDENT;
}

inline std::string options_approval_requester_to_string(OptionsApprovalRequester r) {
    switch (r) {
        case OptionsApprovalRequester::ALPACA_ADMIN:   return "ALPACA_ADMIN";
        case OptionsApprovalRequester::CORRESPONDENT:  return "CORRESPONDENT";
        default:                                       return "CORRESPONDENT";
    }
}

// ---------------------------------------------------------------------------
// OptionsApproval — model for options approval request/response
// ---------------------------------------------------------------------------

struct OptionsApproval {
    std::string id;
    std::string account_id;
    DateTime created_at;
    DateTime updated_at;
    int requested_level = 0;
    int approved_level = 0;
    OptionsApprovalStatus status = OptionsApprovalStatus::UNSET;
    OptionsApprovalRequester requester = OptionsApprovalRequester::CORRESPONDENT;

    OptionsApproval() = default;
    OptionsApproval(const json& j)
        : id(parse_string(j, "id")),
          account_id(parse_string(j, "account_id")),
          created_at(parse_datetime(j, "created_at")),
          updated_at(parse_datetime(j, "updated_at")),
          requested_level(parse_int(j, "requested_level")),
          approved_level(parse_int(j, "approved_level")),
          status(options_approval_status_from_string(parse_string(j, "status"))),
          requester(options_approval_requester_from_string(parse_string(j, "requester")))
    {}

    std::string to_string() const {
        std::string s = "OptionsApproval { id=" + id
            + ", account_id=" + account_id
            + ", requested_level=" + std::to_string(requested_level)
            + ", approved_level=" + std::to_string(approved_level)
            + ", status=" + options_approval_status_to_string(status)
            + ", requester=" + options_approval_requester_to_string(requester)
            + " }";
        return s;
    }
};

} // namespace alpaca
