#pragma once

#include <optional>
#include <string>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

/// Document types for system-generated account documents (tax statements,
/// trade confirmations, etc.) returned by GET /v1/accounts/{id}/documents.
enum class BrokerAccountDocumentType {
    UNSET,
    ACCOUNT_APPLICATION,
    ACCOUNT_STATEMENT,
    CRYPTO_ACCOUNT_STATEMENT,
    OPTIONS_APPLICATION,
    TAX_STATEMENT,
    TRADE_CONFIRMATION_JSON,
    TRADE_CONFIRMATION
};

inline BrokerAccountDocumentType broker_account_document_type_from_string(const std::string& s) {
    if (s == "account_application")       return BrokerAccountDocumentType::ACCOUNT_APPLICATION;
    if (s == "account_statement")         return BrokerAccountDocumentType::ACCOUNT_STATEMENT;
    if (s == "crypto_account_statement")  return BrokerAccountDocumentType::CRYPTO_ACCOUNT_STATEMENT;
    if (s == "options_application")       return BrokerAccountDocumentType::OPTIONS_APPLICATION;
    if (s == "tax_statement")             return BrokerAccountDocumentType::TAX_STATEMENT;
    if (s == "trade_confirmation_json")   return BrokerAccountDocumentType::TRADE_CONFIRMATION_JSON;
    if (s == "trade_confirmation")        return BrokerAccountDocumentType::TRADE_CONFIRMATION;
    return BrokerAccountDocumentType::UNSET;
}

inline std::string broker_account_document_type_to_string(BrokerAccountDocumentType t) {
    switch (t) {
        case BrokerAccountDocumentType::ACCOUNT_APPLICATION:       return "account_application";
        case BrokerAccountDocumentType::ACCOUNT_STATEMENT:         return "account_statement";
        case BrokerAccountDocumentType::CRYPTO_ACCOUNT_STATEMENT:  return "crypto_account_statement";
        case BrokerAccountDocumentType::OPTIONS_APPLICATION:       return "options_application";
        case BrokerAccountDocumentType::TAX_STATEMENT:             return "tax_statement";
        case BrokerAccountDocumentType::TRADE_CONFIRMATION_JSON:   return "trade_confirmation_json";
        case BrokerAccountDocumentType::TRADE_CONFIRMATION:        return "trade_confirmation";
        case BrokerAccountDocumentType::UNSET:                     return "";
    }
    return "";
}

// ---------------------------------------------------------------------------
// BrokerAccountDocument — response model for the account-documents endpoints
// ---------------------------------------------------------------------------

/// Represents a document returned by:
///   - GET  /v1/accounts/{id}/documents          (list)
///   - POST /v1/accounts/{id}/documents/upload    (upload owner doc)
///   - GET  /v1/accounts/{id}/documents/{doc_id}/download
///   - GET  /v1/accounts/{id}/documents/w8ben/{doc_id}/download
///
/// List responses use `BrokerAccountDocumentType` in `document_type`;
/// upload responses use `BrokerDocumentType` (owner document types) which is
/// parsed into `owner_document_type`.  Both share the same struct shape.
struct BrokerAccountDocument {
    std::string id;
    std::string document_type;
    std::string document_sub_type;
    std::string mime_type;
    DateTime created_at;

    BrokerAccountDocument() = default;
    BrokerAccountDocument(const json& j)
        : id(parse_string(j, "id")),
          document_type(parse_string(j, "document_type")),
          document_sub_type(parse_string(j, "document_sub_type")),
          mime_type(parse_string(j, "mime_type")),
          created_at(parse_datetime(j, "created_at"))
    {}

    std::string to_string() const {
        std::string s = "BrokerAccountDocument { id=" + id
            + ", document_type=" + document_type
            + ", document_sub_type=" + document_sub_type
            + ", mime_type=" + mime_type
            + ", created_at=" + created_at.to_iso_string()
            + " }";
        return s;
    }
};

// ---------------------------------------------------------------------------
// BrokerOwnerDocumentType — document types for owner-uploaded documents
// (POST /v1/accounts/{id}/documents/upload)
// ---------------------------------------------------------------------------

enum class BrokerOwnerDocumentType {
    UNSET,
    ACCOUNT_APPROVAL_LETTER,
    ADDRESS_VERIFICATION,
    CIP_RESULT,
    COMPANY_FORMATION,
    DATE_OF_BIRTH_VERIFICATION,
    ENTITY_OPERATING_DOCUMENT,
    ENTITY_REGISTRATION,
    HIO_DECLARATION_FORM,
    IDENTITY_VERIFICATION,
    LIMITED_TRADING_AUTHORIZATION,
    PEP_DECLARATION_FORM,
    TAX_ID_VERIFICATION,
    W8BEN,
    W9
};

inline std::string broker_owner_document_type_to_string(BrokerOwnerDocumentType t) {
    switch (t) {
        case BrokerOwnerDocumentType::ACCOUNT_APPROVAL_LETTER:       return "account_approval_letter";
        case BrokerOwnerDocumentType::ADDRESS_VERIFICATION:          return "address_verification";
        case BrokerOwnerDocumentType::CIP_RESULT:                    return "cip_result";
        case BrokerOwnerDocumentType::COMPANY_FORMATION:             return "company_formation";
        case BrokerOwnerDocumentType::DATE_OF_BIRTH_VERIFICATION:    return "date_of_birth_verification";
        case BrokerOwnerDocumentType::ENTITY_OPERATING_DOCUMENT:     return "entity_operating_document";
        case BrokerOwnerDocumentType::ENTITY_REGISTRATION:           return "entity_registration";
        case BrokerOwnerDocumentType::HIO_DECLARATION_FORM:          return "hio_declaration_form";
        case BrokerOwnerDocumentType::IDENTITY_VERIFICATION:         return "identity_verification";
        case BrokerOwnerDocumentType::LIMITED_TRADING_AUTHORIZATION: return "limited_trading_authorization";
        case BrokerOwnerDocumentType::PEP_DECLARATION_FORM:          return "pep_declaration_form";
        case BrokerOwnerDocumentType::TAX_ID_VERIFICATION:           return "tax_id_verification";
        case BrokerOwnerDocumentType::W8BEN:                         return "w8ben";
        case BrokerOwnerDocumentType::W9:                            return "w9";
        case BrokerOwnerDocumentType::UNSET:                         return "";
    }
    return "";
}

// ---------------------------------------------------------------------------
// W8benDocument — structured content for w8ben uploads (used as content_data)
// ---------------------------------------------------------------------------

struct W8benDocument {
    // Required fields
    std::string country_citizen;
    std::string date;
    std::string date_of_birth;
    std::string full_name;
    std::string ip_address;
    std::string permanent_address_city_state;
    std::string permanent_address_country;
    std::string permanent_address_street;
    std::string revision;
    std::string timestamp;
    std::string signer_full_name;

    // Optional fields
    std::string additional_conditions;
    std::string foreign_tax_id;
    bool ftin_not_required = false;
    std::string mailing_address_city_state;
    std::string mailing_address_country;
    std::string mailing_address_street;
    std::string paragraph_number;
    int percent_rate_withholding = -1;
    std::string reference_number;
    std::string residency;
    std::string tax_id_ssn;
    std::string income_type;

    json to_json() const {
        json j;
        j["country_citizen"]               = country_citizen;
        j["date"]                          = date;
        j["date_of_birth"]                 = date_of_birth;
        j["full_name"]                     = full_name;
        j["ip_address"]                    = ip_address;
        j["permanent_address_city_state"]  = permanent_address_city_state;
        j["permanent_address_country"]     = permanent_address_country;
        j["permanent_address_street"]      = permanent_address_street;
        j["revision"]                      = revision;
        j["timestamp"]                     = timestamp;
        j["signer_full_name"]              = signer_full_name;

        if (!additional_conditions.empty())       j["additional_conditions"]      = additional_conditions;
        if (!foreign_tax_id.empty())              j["foreign_tax_id"]             = foreign_tax_id;
        if (ftin_not_required)                    j["ftin_not_required"]          = ftin_not_required;
        if (!mailing_address_city_state.empty())  j["mailing_address_city_state"] = mailing_address_city_state;
        if (!mailing_address_country.empty())     j["mailing_address_country"]    = mailing_address_country;
        if (!mailing_address_street.empty())      j["mailing_address_street"]     = mailing_address_street;
        if (!paragraph_number.empty())            j["paragraph_number"]           = paragraph_number;
        if (percent_rate_withholding >= 0)        j["percent_rate_withholding"]   = percent_rate_withholding;
        if (!reference_number.empty())            j["reference_number"]           = reference_number;
        if (!residency.empty())                   j["residency"]                  = residency;
        if (!tax_id_ssn.empty())                  j["tax_id_ssn"]                 = tax_id_ssn;
        if (!income_type.empty())                 j["income_type"]                = income_type;

        return j;
    }
};

// ---------------------------------------------------------------------------
// BrokerDocumentUploadRequest — one entry in the upload request array
// (POST /v1/accounts/{id}/documents/upload)
//
// Either `content` + `mime_type` OR `content_data` must be provided.
// `content_data` is only valid when document_type == W8BEN.
// ---------------------------------------------------------------------------

struct BrokerDocumentUploadRequest {
    BrokerOwnerDocumentType document_type = BrokerOwnerDocumentType::UNSET;
    std::string document_sub_type;
    std::string content;    // base64-encoded file bytes
    std::string mime_type;  // required when content is set
    std::optional<W8benDocument> content_data;

    json to_json() const {
        json j;
        j["document_type"] = broker_owner_document_type_to_string(document_type);
        if (!document_sub_type.empty()) j["document_sub_type"] = document_sub_type;
        if (!content.empty())           j["content"]           = content;
        if (!mime_type.empty())         j["mime_type"]         = mime_type;
        if (content_data)               j["content_data"]      = content_data->to_json();
        return j;
    }
};

} // namespace alpaca
