#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

enum class BrokerAccountStatus {
    INACTIVE,
    ONBOARDING,
    SUBMITTED,
    SUBMISSION_FAILED,
    ACTION_REQUIRED,
    ACCOUNT_UPDATED,
    APPROVAL_PENDING,
    APPROVED,
    REJECTED,
    ACTIVE,
    ACCOUNT_CLOSED
};

inline BrokerAccountStatus broker_account_status_from_string(const std::string& s) {
    if (s == "INACTIVE") return BrokerAccountStatus::INACTIVE;
    if (s == "ONBOARDING") return BrokerAccountStatus::ONBOARDING;
    if (s == "SUBMITTED") return BrokerAccountStatus::SUBMITTED;
    if (s == "SUBMISSION_FAILED") return BrokerAccountStatus::SUBMISSION_FAILED;
    if (s == "ACTION_REQUIRED") return BrokerAccountStatus::ACTION_REQUIRED;
    if (s == "ACCOUNT_UPDATED") return BrokerAccountStatus::ACCOUNT_UPDATED;
    if (s == "APPROVAL_PENDING") return BrokerAccountStatus::APPROVAL_PENDING;
    if (s == "APPROVED") return BrokerAccountStatus::APPROVED;
    if (s == "REJECTED") return BrokerAccountStatus::REJECTED;
    if (s == "ACTIVE") return BrokerAccountStatus::ACTIVE;
    if (s == "ACCOUNT_CLOSED") return BrokerAccountStatus::ACCOUNT_CLOSED;
    return BrokerAccountStatus::INACTIVE;
}

inline std::string broker_account_status_to_string(BrokerAccountStatus status) {
    switch (status) {
        case BrokerAccountStatus::INACTIVE: return "INACTIVE";
        case BrokerAccountStatus::ONBOARDING: return "ONBOARDING";
        case BrokerAccountStatus::SUBMITTED: return "SUBMITTED";
        case BrokerAccountStatus::SUBMISSION_FAILED: return "SUBMISSION_FAILED";
        case BrokerAccountStatus::ACTION_REQUIRED: return "ACTION_REQUIRED";
        case BrokerAccountStatus::ACCOUNT_UPDATED: return "ACCOUNT_UPDATED";
        case BrokerAccountStatus::APPROVAL_PENDING: return "APPROVAL_PENDING";
        case BrokerAccountStatus::APPROVED: return "APPROVED";
        case BrokerAccountStatus::REJECTED: return "REJECTED";
        case BrokerAccountStatus::ACTIVE: return "ACTIVE";
        case BrokerAccountStatus::ACCOUNT_CLOSED: return "ACCOUNT_CLOSED";
        default: return "INACTIVE";
    }
}

enum class BrokerCryptoStatus {
    INACTIVE,
    ONBOARDING,
    SUBMITTED,
    SUBMISSION_FAILED,
    ACTION_REQUIRED,
    ACCOUNT_UPDATED,
    APPROVAL_PENDING,
    APPROVED,
    REJECTED,
    ACTIVE,
    ACCOUNT_CLOSED
};

inline BrokerCryptoStatus broker_crypto_status_from_string(const std::string& s) {
    if (s == "ONBOARDING") return BrokerCryptoStatus::ONBOARDING;
    if (s == "SUBMITTED") return BrokerCryptoStatus::SUBMITTED;
    if (s == "SUBMISSION_FAILED") return BrokerCryptoStatus::SUBMISSION_FAILED;
    if (s == "ACTION_REQUIRED") return BrokerCryptoStatus::ACTION_REQUIRED;
    if (s == "ACCOUNT_UPDATED") return BrokerCryptoStatus::ACCOUNT_UPDATED;
    if (s == "APPROVAL_PENDING") return BrokerCryptoStatus::APPROVAL_PENDING;
    if (s == "APPROVED") return BrokerCryptoStatus::APPROVED;
    if (s == "REJECTED") return BrokerCryptoStatus::REJECTED;
    if (s == "ACTIVE") return BrokerCryptoStatus::ACTIVE;
    if (s == "ACCOUNT_CLOSED") return BrokerCryptoStatus::ACCOUNT_CLOSED;
    return BrokerCryptoStatus::INACTIVE;
}

inline std::string broker_crypto_status_to_string(BrokerCryptoStatus s) {
    switch (s) {
        case BrokerCryptoStatus::ONBOARDING: return "ONBOARDING";
        case BrokerCryptoStatus::SUBMITTED: return "SUBMITTED";
        case BrokerCryptoStatus::SUBMISSION_FAILED: return "SUBMISSION_FAILED";
        case BrokerCryptoStatus::ACTION_REQUIRED: return "ACTION_REQUIRED";
        case BrokerCryptoStatus::ACCOUNT_UPDATED: return "ACCOUNT_UPDATED";
        case BrokerCryptoStatus::APPROVAL_PENDING: return "APPROVAL_PENDING";
        case BrokerCryptoStatus::APPROVED: return "APPROVED";
        case BrokerCryptoStatus::REJECTED: return "REJECTED";
        case BrokerCryptoStatus::ACTIVE: return "ACTIVE";
        case BrokerCryptoStatus::ACCOUNT_CLOSED: return "ACCOUNT_CLOSED";
        default: return "INACTIVE";
    }
}

enum class TaxIdType {
    USA_SSN,
    USA_ITIN,
    ARG_AG_CUIT,
    AUS_TFN,
    AUS_ABN,
    BOL_NIT,
    BRA_CPF,
    CHL_RUT,
    COL_NIT,
    CRI_NITE,
    DEU_TAX_ID,
    DOM_RNC,
    ECU_RUC,
    FRA_SPI,
    GBR_UTR,
    GBR_NINO,
    GTM_NIT,
    HND_RTN,
    HUN_TIN,
    IDN_KTP,
    IND_PAN,
    ISR_TAX_ID,
    ITA_TAX_ID,
    JPN_TAX_ID,
    MEX_RFC,
    NIC_RUC,
    NLD_TIN,
    PAN_RUC,
    PER_RUC,
    PRY_RUC,
    SGP_NRIC,
    SGP_FIN,
    SGP_ASGD,
    SGP_ITR,
    SLV_NIT,
    SWE_TAX_ID,
    URY_RUT,
    VEN_RIF,
    NATIONAL_ID,
    PASSPORT,
    PERMANENT_RESIDENT,
    DRIVER_LICENSE,
    OTHER_GOV_ID,
    NOT_SPECIFIED
};

inline TaxIdType tax_id_type_from_string(const std::string& s) {
    if (s == "USA_SSN") return TaxIdType::USA_SSN;
    if (s == "USA_ITIN") return TaxIdType::USA_ITIN;
    if (s == "ARG_AG_CUIT") return TaxIdType::ARG_AG_CUIT;
    if (s == "AUS_TFN") return TaxIdType::AUS_TFN;
    if (s == "AUS_ABN") return TaxIdType::AUS_ABN;
    if (s == "BOL_NIT") return TaxIdType::BOL_NIT;
    if (s == "BRA_CPF") return TaxIdType::BRA_CPF;
    if (s == "CHL_RUT") return TaxIdType::CHL_RUT;
    if (s == "COL_NIT") return TaxIdType::COL_NIT;
    if (s == "CRI_NITE") return TaxIdType::CRI_NITE;
    if (s == "DEU_TAX_ID") return TaxIdType::DEU_TAX_ID;
    if (s == "DOM_RNC") return TaxIdType::DOM_RNC;
    if (s == "ECU_RUC") return TaxIdType::ECU_RUC;
    if (s == "FRA_SPI") return TaxIdType::FRA_SPI;
    if (s == "GBR_UTR") return TaxIdType::GBR_UTR;
    if (s == "GBR_NINO") return TaxIdType::GBR_NINO;
    if (s == "GTM_NIT") return TaxIdType::GTM_NIT;
    if (s == "HND_RTN") return TaxIdType::HND_RTN;
    if (s == "HUN_TIN") return TaxIdType::HUN_TIN;
    if (s == "IDN_KTP") return TaxIdType::IDN_KTP;
    if (s == "IND_PAN") return TaxIdType::IND_PAN;
    if (s == "ISR_TAX_ID") return TaxIdType::ISR_TAX_ID;
    if (s == "ITA_TAX_ID") return TaxIdType::ITA_TAX_ID;
    if (s == "JPN_TAX_ID") return TaxIdType::JPN_TAX_ID;
    if (s == "MEX_RFC") return TaxIdType::MEX_RFC;
    if (s == "NIC_RUC") return TaxIdType::NIC_RUC;
    if (s == "NLD_TIN") return TaxIdType::NLD_TIN;
    if (s == "PAN_RUC") return TaxIdType::PAN_RUC;
    if (s == "PER_RUC") return TaxIdType::PER_RUC;
    if (s == "PRY_RUC") return TaxIdType::PRY_RUC;
    if (s == "SGP_NRIC") return TaxIdType::SGP_NRIC;
    if (s == "SGP_FIN") return TaxIdType::SGP_FIN;
    if (s == "SGP_ASGD") return TaxIdType::SGP_ASGD;
    if (s == "SGP_ITR") return TaxIdType::SGP_ITR;
    if (s == "SLV_NIT") return TaxIdType::SLV_NIT;
    if (s == "SWE_TAX_ID") return TaxIdType::SWE_TAX_ID;
    if (s == "URY_RUT") return TaxIdType::URY_RUT;
    if (s == "VEN_RIF") return TaxIdType::VEN_RIF;
    if (s == "NATIONAL_ID") return TaxIdType::NATIONAL_ID;
    if (s == "PASSPORT") return TaxIdType::PASSPORT;
    if (s == "PERMANENT_RESIDENT") return TaxIdType::PERMANENT_RESIDENT;
    if (s == "DRIVER_LICENSE") return TaxIdType::DRIVER_LICENSE;
    if (s == "OTHER_GOV_ID") return TaxIdType::OTHER_GOV_ID;
    return TaxIdType::NOT_SPECIFIED;
}

inline std::string tax_id_type_to_string(TaxIdType t) {
    switch (t) {
        case TaxIdType::USA_SSN: return "USA_SSN";
        case TaxIdType::USA_ITIN: return "USA_ITIN";
        case TaxIdType::ARG_AG_CUIT: return "ARG_AG_CUIT";
        case TaxIdType::AUS_TFN: return "AUS_TFN";
        case TaxIdType::AUS_ABN: return "AUS_ABN";
        case TaxIdType::BOL_NIT: return "BOL_NIT";
        case TaxIdType::BRA_CPF: return "BRA_CPF";
        case TaxIdType::CHL_RUT: return "CHL_RUT";
        case TaxIdType::COL_NIT: return "COL_NIT";
        case TaxIdType::CRI_NITE: return "CRI_NITE";
        case TaxIdType::DEU_TAX_ID: return "DEU_TAX_ID";
        case TaxIdType::DOM_RNC: return "DOM_RNC";
        case TaxIdType::ECU_RUC: return "ECU_RUC";
        case TaxIdType::FRA_SPI: return "FRA_SPI";
        case TaxIdType::GBR_UTR: return "GBR_UTR";
        case TaxIdType::GBR_NINO: return "GBR_NINO";
        case TaxIdType::GTM_NIT: return "GTM_NIT";
        case TaxIdType::HND_RTN: return "HND_RTN";
        case TaxIdType::HUN_TIN: return "HUN_TIN";
        case TaxIdType::IDN_KTP: return "IDN_KTP";
        case TaxIdType::IND_PAN: return "IND_PAN";
        case TaxIdType::ISR_TAX_ID: return "ISR_TAX_ID";
        case TaxIdType::ITA_TAX_ID: return "ITA_TAX_ID";
        case TaxIdType::JPN_TAX_ID: return "JPN_TAX_ID";
        case TaxIdType::MEX_RFC: return "MEX_RFC";
        case TaxIdType::NIC_RUC: return "NIC_RUC";
        case TaxIdType::NLD_TIN: return "NLD_TIN";
        case TaxIdType::PAN_RUC: return "PAN_RUC";
        case TaxIdType::PER_RUC: return "PER_RUC";
        case TaxIdType::PRY_RUC: return "PRY_RUC";
        case TaxIdType::SGP_NRIC: return "SGP_NRIC";
        case TaxIdType::SGP_FIN: return "SGP_FIN";
        case TaxIdType::SGP_ASGD: return "SGP_ASGD";
        case TaxIdType::SGP_ITR: return "SGP_ITR";
        case TaxIdType::SLV_NIT: return "SLV_NIT";
        case TaxIdType::SWE_TAX_ID: return "SWE_TAX_ID";
        case TaxIdType::URY_RUT: return "URY_RUT";
        case TaxIdType::VEN_RIF: return "VEN_RIF";
        case TaxIdType::NATIONAL_ID: return "NATIONAL_ID";
        case TaxIdType::PASSPORT: return "PASSPORT";
        case TaxIdType::PERMANENT_RESIDENT: return "PERMANENT_RESIDENT";
        case TaxIdType::DRIVER_LICENSE: return "DRIVER_LICENSE";
        case TaxIdType::OTHER_GOV_ID: return "OTHER_GOV_ID";
        default: return "NOT_SPECIFIED";
    }
}

enum class BrokerAgreementType {
    MARGIN_AGREEMENT,
    ACCOUNT_AGREEMENT,
    CUSTOMER_AGREEMENT,
    CRYPTO_AGREEMENT,
    OPTIONS_AGREEMENT
};

inline BrokerAgreementType broker_agreement_type_from_string(const std::string& s) {
    if (s == "margin_agreement") return BrokerAgreementType::MARGIN_AGREEMENT;
    if (s == "account_agreement") return BrokerAgreementType::ACCOUNT_AGREEMENT;
    if (s == "customer_agreement") return BrokerAgreementType::CUSTOMER_AGREEMENT;
    if (s == "crypto_agreement") return BrokerAgreementType::CRYPTO_AGREEMENT;
    if (s == "options_agreement") return BrokerAgreementType::OPTIONS_AGREEMENT;
    return BrokerAgreementType::CUSTOMER_AGREEMENT;
}

inline std::string broker_agreement_type_to_string(BrokerAgreementType a) {
    switch (a) {
        case BrokerAgreementType::MARGIN_AGREEMENT: return "margin_agreement";
        case BrokerAgreementType::ACCOUNT_AGREEMENT: return "account_agreement";
        case BrokerAgreementType::CUSTOMER_AGREEMENT: return "customer_agreement";
        case BrokerAgreementType::CRYPTO_AGREEMENT: return "crypto_agreement";
        case BrokerAgreementType::OPTIONS_AGREEMENT: return "options_agreement";
        default: return "customer_agreement";
    }
}

enum class BrokerDocumentType {
    IDENTITY_VERIFICATION,
    ADDRESS_VERIFICATION,
    DATE_OF_BIRTH_VERIFICATION,
    TAX_ID_VERIFICATION,
    ACCOUNT_APPROVAL_LETTER,
    W8BEN,
    W9,
    CIP_RESULT,
    COMPANY_FORMATION,
    ENTITY_OPERATING_DOCUMENT,
    ENTITY_REGISTRATION,
    HIO_DECLARATION_FORM,
    LIMITED_TRADING_AUTHORIZATION,
    PEP_DECLARATION_FORM,
    OTHER
};

inline BrokerDocumentType broker_document_type_from_string(const std::string& s) {
    if (s == "identity_verification") return BrokerDocumentType::IDENTITY_VERIFICATION;
    if (s == "address_verification") return BrokerDocumentType::ADDRESS_VERIFICATION;
    if (s == "date_of_birth_verification") return BrokerDocumentType::DATE_OF_BIRTH_VERIFICATION;
    if (s == "tax_id_verification") return BrokerDocumentType::TAX_ID_VERIFICATION;
    if (s == "account_approval_letter") return BrokerDocumentType::ACCOUNT_APPROVAL_LETTER;
    if (s == "w8ben") return BrokerDocumentType::W8BEN;
    if (s == "w9") return BrokerDocumentType::W9;
    if (s == "cip_result") return BrokerDocumentType::CIP_RESULT;
    if (s == "company_formation") return BrokerDocumentType::COMPANY_FORMATION;
    if (s == "entity_operating_document") return BrokerDocumentType::ENTITY_OPERATING_DOCUMENT;
    if (s == "entity_registration") return BrokerDocumentType::ENTITY_REGISTRATION;
    if (s == "hio_declaration_form") return BrokerDocumentType::HIO_DECLARATION_FORM;
    if (s == "limited_trading_authorization") return BrokerDocumentType::LIMITED_TRADING_AUTHORIZATION;
    if (s == "pep_declaration_form") return BrokerDocumentType::PEP_DECLARATION_FORM;
    return BrokerDocumentType::OTHER;
}

inline std::string broker_document_type_to_string(BrokerDocumentType d) {
    switch (d) {
        case BrokerDocumentType::IDENTITY_VERIFICATION: return "identity_verification";
        case BrokerDocumentType::ADDRESS_VERIFICATION: return "address_verification";
        case BrokerDocumentType::DATE_OF_BIRTH_VERIFICATION: return "date_of_birth_verification";
        case BrokerDocumentType::TAX_ID_VERIFICATION: return "tax_id_verification";
        case BrokerDocumentType::ACCOUNT_APPROVAL_LETTER: return "account_approval_letter";
        case BrokerDocumentType::W8BEN: return "w8ben";
        case BrokerDocumentType::W9: return "w9";
        case BrokerDocumentType::CIP_RESULT: return "cip_result";
        case BrokerDocumentType::COMPANY_FORMATION: return "company_formation";
        case BrokerDocumentType::ENTITY_OPERATING_DOCUMENT: return "entity_operating_document";
        case BrokerDocumentType::ENTITY_REGISTRATION: return "entity_registration";
        case BrokerDocumentType::HIO_DECLARATION_FORM: return "hio_declaration_form";
        case BrokerDocumentType::LIMITED_TRADING_AUTHORIZATION: return "limited_trading_authorization";
        case BrokerDocumentType::PEP_DECLARATION_FORM: return "pep_declaration_form";
        default: return "other";
    }
}

enum class BrokerAccountType {
    TRADING,
    CUSTODIAL,
    DONOR_ADVISED,
    IRA
};

inline BrokerAccountType broker_account_type_from_string(const std::string& s) {
    if (s == "trading") return BrokerAccountType::TRADING;
    if (s == "custodial") return BrokerAccountType::CUSTODIAL;
    if (s == "donor_advised") return BrokerAccountType::DONOR_ADVISED;
    if (s == "ira") return BrokerAccountType::IRA;
    return BrokerAccountType::TRADING;
}

inline std::string broker_account_type_to_string(BrokerAccountType t) {
    switch (t) {
        case BrokerAccountType::TRADING: return "trading";
        case BrokerAccountType::CUSTODIAL: return "custodial";
        case BrokerAccountType::DONOR_ADVISED: return "donor_advised";
        case BrokerAccountType::IRA: return "ira";
        default: return "trading";
    }
}

// ---------------------------------------------------------------------------
// Nested structs
// ---------------------------------------------------------------------------

struct KYCResults {
    std::string reject;
    std::string accept;
    std::string indeterminate;
    std::string additional_information;
    std::string summary;

    KYCResults() = default;
    KYCResults(const json& j)
        : reject(parse_string(j, "reject")),
          accept(parse_string(j, "accept")),
          indeterminate(parse_string(j, "indeterminate")),
          additional_information(parse_string(j, "additional_information")),
          summary(parse_string(j, "summary")) {}

    json to_json() const {
        json j;
        j["reject"] = reject;
        j["accept"] = accept;
        j["indeterminate"] = indeterminate;
        j["additional_information"] = additional_information;
        j["summary"] = summary;
        return j;
    }
};

struct BrokerContact {
    std::string email_address;
    std::string phone_number;
    std::vector<std::string> street_address;
    std::string unit;
    std::string city;
    std::string state;
    std::string country;
    std::string postal_code;

    BrokerContact() = default;
    BrokerContact(const json& j)
        : email_address(parse_string(j, "email_address")),
          phone_number(parse_string(j, "phone_number")),
          unit(parse_string(j, "unit")),
          city(parse_string(j, "city")),
          state(parse_string(j, "state")),
          country(parse_string(j, "country")),
          postal_code(parse_string(j, "postal_code"))
    {
        if (j.contains("street_address") && j["street_address"].is_array()) {
            street_address = j["street_address"].get<std::vector<std::string>>();
        }
    }

    json to_json() const {
        json j;
        j["email_address"] = email_address;
        j["phone_number"] = phone_number;
        j["street_address"] = street_address;
        if (!unit.empty()) j["unit"] = unit;
        j["city"] = city;
        if (!state.empty()) j["state"] = state;
        if (!country.empty()) j["country"] = country;
        if (!postal_code.empty()) j["postal_code"] = postal_code;
        return j;
    }
};

struct BrokerIdentity {
    std::string given_name;
    std::string family_name;
    std::string middle_name;
    std::string date_of_birth;
    std::string tax_id;
    TaxIdType tax_id_type = TaxIdType::NOT_SPECIFIED;
    std::string country_of_citizenship;
    std::string country_of_birth;
    std::string country_of_tax_residence;
    std::vector<std::string> funding_source;
    std::string visa_type;
    std::string visa_expiration_date;
    std::string date_of_departure_from_usa;
    bool permanent_resident = false;
    std::string marital_status;
    int number_of_dependents = 0;
    double annual_income_min = 0;
    double annual_income_max = 0;
    double liquid_net_worth_min = 0;
    double liquid_net_worth_max = 0;
    double total_net_worth_min = 0;
    double total_net_worth_max = 0;
    std::string investment_experience_with_stocks;
    std::string investment_experience_with_options;

    BrokerIdentity() = default;
    BrokerIdentity(const json& j)
        : given_name(parse_string(j, "given_name")),
          family_name(parse_string(j, "family_name")),
          middle_name(parse_string(j, "middle_name")),
          date_of_birth(parse_string(j, "date_of_birth")),
          tax_id(parse_string(j, "tax_id")),
          tax_id_type(tax_id_type_from_string(parse_string(j, "tax_id_type"))),
          country_of_citizenship(parse_string(j, "country_of_citizenship")),
          country_of_birth(parse_string(j, "country_of_birth")),
          country_of_tax_residence(parse_string(j, "country_of_tax_residence")),
          visa_type(parse_string(j, "visa_type")),
          visa_expiration_date(parse_string(j, "visa_expiration_date")),
          date_of_departure_from_usa(parse_string(j, "date_of_departure_from_usa")),
          permanent_resident(parse_bool(j, "permanent_resident")),
          marital_status(parse_string(j, "marital_status")),
          number_of_dependents(parse_int(j, "number_of_dependents")),
          investment_experience_with_stocks(parse_string(j, "investment_experience_with_stocks")),
          investment_experience_with_options(parse_string(j, "investment_experience_with_options"))
    {
        if (j.contains("funding_source") && j["funding_source"].is_array()) {
            funding_source = j["funding_source"].get<std::vector<std::string>>();
        }
        if (j.contains("annual_income_min") && !j["annual_income_min"].is_null()) {
            annual_income_min = j["annual_income_min"].get<double>();
        }
        if (j.contains("annual_income_max") && !j["annual_income_max"].is_null()) {
            annual_income_max = j["annual_income_max"].get<double>();
        }
        if (j.contains("liquid_net_worth_min") && !j["liquid_net_worth_min"].is_null()) {
            liquid_net_worth_min = j["liquid_net_worth_min"].get<double>();
        }
        if (j.contains("liquid_net_worth_max") && !j["liquid_net_worth_max"].is_null()) {
            liquid_net_worth_max = j["liquid_net_worth_max"].get<double>();
        }
        if (j.contains("total_net_worth_min") && !j["total_net_worth_min"].is_null()) {
            total_net_worth_min = j["total_net_worth_min"].get<double>();
        }
        if (j.contains("total_net_worth_max") && !j["total_net_worth_max"].is_null()) {
            total_net_worth_max = j["total_net_worth_max"].get<double>();
        }
    }

    json to_json() const {
        json j;
        j["given_name"] = given_name;
        j["family_name"] = family_name;
        if (!middle_name.empty()) j["middle_name"] = middle_name;
        j["date_of_birth"] = date_of_birth;
        if (!tax_id.empty()) j["tax_id"] = tax_id;
        j["tax_id_type"] = tax_id_type_to_string(tax_id_type);
        if (!country_of_citizenship.empty()) j["country_of_citizenship"] = country_of_citizenship;
        if (!country_of_birth.empty()) j["country_of_birth"] = country_of_birth;
        j["country_of_tax_residence"] = country_of_tax_residence;
        j["funding_source"] = funding_source;
        if (!visa_type.empty()) j["visa_type"] = visa_type;
        if (!visa_expiration_date.empty()) j["visa_expiration_date"] = visa_expiration_date;
        if (!date_of_departure_from_usa.empty()) j["date_of_departure_from_usa"] = date_of_departure_from_usa;
        if (!marital_status.empty()) j["marital_status"] = marital_status;
        if (!investment_experience_with_stocks.empty()) j["investment_experience_with_stocks"] = investment_experience_with_stocks;
        if (!investment_experience_with_options.empty()) j["investment_experience_with_options"] = investment_experience_with_options;
        return j;
    }
};

struct BrokerDisclosureContext {
    std::string context_type;
    std::string company_name;
    std::string company_street_address;
    std::string company_city;
    std::string company_state;
    std::string company_country;
    std::string company_compliance_email;
    std::string given_name;
    std::string family_name;

    BrokerDisclosureContext() = default;
    BrokerDisclosureContext(const json& j)
        : context_type(parse_string(j, "context_type")),
          company_name(parse_string(j, "company_name")),
          company_street_address(parse_string(j, "company_street_address")),
          company_city(parse_string(j, "company_city")),
          company_state(parse_string(j, "company_state")),
          company_country(parse_string(j, "company_country")),
          company_compliance_email(parse_string(j, "company_compliance_email")),
          given_name(parse_string(j, "given_name")),
          family_name(parse_string(j, "family_name")) {}

    json to_json() const {
        json j;
        j["context_type"] = context_type;
        if (!company_name.empty()) j["company_name"] = company_name;
        if (!company_street_address.empty()) j["company_street_address"] = company_street_address;
        if (!company_city.empty()) j["company_city"] = company_city;
        if (!company_state.empty()) j["company_state"] = company_state;
        if (!company_country.empty()) j["company_country"] = company_country;
        if (!company_compliance_email.empty()) j["company_compliance_email"] = company_compliance_email;
        if (!given_name.empty()) j["given_name"] = given_name;
        if (!family_name.empty()) j["family_name"] = family_name;
        return j;
    }
};

struct BrokerDisclosures {
    bool is_control_person = false;
    bool is_affiliated_exchange_or_finra = false;
    bool is_politically_exposed = false;
    bool immediate_family_exposed = false;
    std::string employment_status;
    std::string employer_name;
    std::string employer_address;
    std::string employment_position;
    std::string employment_sector;
    std::vector<BrokerDisclosureContext> context;

    BrokerDisclosures() = default;
    BrokerDisclosures(const json& j)
        : is_control_person(parse_bool(j, "is_control_person")),
          is_affiliated_exchange_or_finra(parse_bool(j, "is_affiliated_exchange_or_finra")),
          is_politically_exposed(parse_bool(j, "is_politically_exposed")),
          immediate_family_exposed(parse_bool(j, "immediate_family_exposed")),
          employment_status(parse_string(j, "employment_status")),
          employer_name(parse_string(j, "employer_name")),
          employer_address(parse_string(j, "employer_address")),
          employment_position(parse_string(j, "employment_position")),
          employment_sector(parse_string(j, "employment_sector"))
    {
        if (j.contains("context") && j["context"].is_array()) {
            for (const auto& item : j["context"]) {
                context.emplace_back(item);
            }
        }
    }

    json to_json() const {
        json j;
        j["is_control_person"] = is_control_person;
        j["is_affiliated_exchange_or_finra"] = is_affiliated_exchange_or_finra;
        j["is_politically_exposed"] = is_politically_exposed;
        j["immediate_family_exposed"] = immediate_family_exposed;
        if (!employment_status.empty()) j["employment_status"] = employment_status;
        if (!employer_name.empty()) j["employer_name"] = employer_name;
        if (!employer_address.empty()) j["employer_address"] = employer_address;
        if (!employment_position.empty()) j["employment_position"] = employment_position;
        if (!employment_sector.empty()) j["employment_sector"] = employment_sector;
        if (!context.empty()) {
            j["context"] = json::array();
            for (const auto& c : context) j["context"].push_back(c.to_json());
        }
        return j;
    }
};

struct BrokerAgreement {
    BrokerAgreementType agreement = BrokerAgreementType::CUSTOMER_AGREEMENT;
    std::string signed_at;
    std::string ip_address;
    std::string revision;

    BrokerAgreement() = default;
    BrokerAgreement(const json& j)
        : agreement(broker_agreement_type_from_string(parse_string(j, "agreement"))),
          signed_at(parse_string(j, "signed_at")),
          ip_address(parse_string(j, "ip_address")),
          revision(parse_string(j, "revision")) {}

    json to_json() const {
        json j;
        j["agreement"] = broker_agreement_type_to_string(agreement);
        j["signed_at"] = signed_at;
        j["ip_address"] = ip_address;
        if (!revision.empty()) j["revision"] = revision;
        return j;
    }
};

struct BrokerDocument {
    std::string id;
    BrokerDocumentType document_type = BrokerDocumentType::OTHER;
    std::string document_sub_type;
    std::string mime_type;
    std::string created_at;

    BrokerDocument() = default;
    BrokerDocument(const json& j)
        : id(parse_string(j, "id")),
          document_type(broker_document_type_from_string(parse_string(j, "document_type"))),
          document_sub_type(parse_string(j, "document_sub_type")),
          mime_type(parse_string(j, "mime_type")),
          created_at(parse_string(j, "created_at")) {}

    json to_json() const {
        json j;
        j["id"] = id;
        j["document_type"] = broker_document_type_to_string(document_type);
        if (!document_sub_type.empty()) j["document_sub_type"] = document_sub_type;
        if (!mime_type.empty()) j["mime_type"] = mime_type;
        j["created_at"] = created_at;
        return j;
    }
};

struct BrokerTrustedContact {
    std::string given_name;
    std::string family_name;
    std::string email_address;
    std::string phone_number;
    std::vector<std::string> street_address;
    std::string city;
    std::string state;
    std::string postal_code;
    std::string country;

    BrokerTrustedContact() = default;
    BrokerTrustedContact(const json& j)
        : given_name(parse_string(j, "given_name")),
          family_name(parse_string(j, "family_name")),
          email_address(parse_string(j, "email_address")),
          phone_number(parse_string(j, "phone_number")),
          city(parse_string(j, "city")),
          state(parse_string(j, "state")),
          postal_code(parse_string(j, "postal_code")),
          country(parse_string(j, "country"))
    {
        if (j.contains("street_address") && j["street_address"].is_array()) {
            street_address = j["street_address"].get<std::vector<std::string>>();
        }
    }

    json to_json() const {
        json j;
        j["given_name"] = given_name;
        j["family_name"] = family_name;
        if (!email_address.empty()) j["email_address"] = email_address;
        if (!phone_number.empty()) j["phone_number"] = phone_number;
        if (!street_address.empty()) j["street_address"] = street_address;
        if (!city.empty()) j["city"] = city;
        if (!state.empty()) j["state"] = state;
        if (!postal_code.empty()) j["postal_code"] = postal_code;
        if (!country.empty()) j["country"] = country;
        return j;
    }
};

struct BrokerMinorIdentity {
    std::string given_name;
    std::string family_name;
    std::string date_of_birth;
    std::string tax_id;
    TaxIdType tax_id_type = TaxIdType::NOT_SPECIFIED;
    std::string country_of_citizenship;
    std::string country_of_birth;
    std::string country_of_tax_residence;
    std::string state;
    std::string email;

    BrokerMinorIdentity() = default;
    BrokerMinorIdentity(const json& j)
        : given_name(parse_string(j, "given_name")),
          family_name(parse_string(j, "family_name")),
          date_of_birth(parse_string(j, "date_of_birth")),
          tax_id(parse_string(j, "tax_id")),
          tax_id_type(tax_id_type_from_string(parse_string(j, "tax_id_type"))),
          country_of_citizenship(parse_string(j, "country_of_citizenship")),
          country_of_birth(parse_string(j, "country_of_birth")),
          country_of_tax_residence(parse_string(j, "country_of_tax_residence")),
          state(parse_string(j, "state")),
          email(parse_string(j, "email")) {}

    json to_json() const {
        json j;
        j["given_name"] = given_name;
        j["family_name"] = family_name;
        j["date_of_birth"] = date_of_birth;
        if (!tax_id.empty()) j["tax_id"] = tax_id;
        j["tax_id_type"] = tax_id_type_to_string(tax_id_type);
        if (!country_of_citizenship.empty()) j["country_of_citizenship"] = country_of_citizenship;
        if (!country_of_birth.empty()) j["country_of_birth"] = country_of_birth;
        if (!country_of_tax_residence.empty()) j["country_of_tax_residence"] = country_of_tax_residence;
        if (!state.empty()) j["state"] = state;
        if (!email.empty()) j["email"] = email;
        return j;
    }
};

struct BrokerTradingConfigurations {
    std::string dtbp_check;
    std::string trade_confirm_email;
    bool suspend_trade = false;
    bool no_shorting = false;
    bool fractional_trading = false;
    std::string max_margin_multiplier;
    int max_options_trading_level = 0;
    std::string pdt_check;
    std::string ptp_no_exception_entry;
    bool disable_overnight_trading = false;

    BrokerTradingConfigurations() = default;
    BrokerTradingConfigurations(const json& j)
        : dtbp_check(parse_string(j, "dtbp_check")),
          trade_confirm_email(parse_string(j, "trade_confirm_email")),
          suspend_trade(parse_bool(j, "suspend_trade")),
          no_shorting(parse_bool(j, "no_shorting")),
          fractional_trading(parse_bool(j, "fractional_trading")),
          max_margin_multiplier(parse_string(j, "max_margin_multiplier")),
          max_options_trading_level(parse_int(j, "max_options_trading_level")),
          pdt_check(parse_string(j, "pdt_check")),
          ptp_no_exception_entry(parse_string(j, "ptp_no_exception_entry")),
          disable_overnight_trading(parse_bool(j, "disable_overnight_trading")) {}

    json to_json() const {
        json j;
        j["dtbp_check"] = dtbp_check;
        j["trade_confirm_email"] = trade_confirm_email;
        j["suspend_trade"] = suspend_trade;
        j["no_shorting"] = no_shorting;
        j["fractional_trading"] = fractional_trading;
        if (!max_margin_multiplier.empty()) j["max_margin_multiplier"] = max_margin_multiplier;
        j["max_options_trading_level"] = max_options_trading_level;
        if (!pdt_check.empty()) j["pdt_check"] = pdt_check;
        if (!ptp_no_exception_entry.empty()) j["ptp_no_exception_entry"] = ptp_no_exception_entry;
        j["disable_overnight_trading"] = disable_overnight_trading;
        return j;
    }
};

struct BrokerBeneficiary {
    std::string given_name;
    std::string middle_name;
    std::string family_name;
    std::string date_of_birth;
    std::string tax_id;
    TaxIdType tax_id_type = TaxIdType::NOT_SPECIFIED;
    std::string relationship;
    std::string type;
    std::string share_pct;

    BrokerBeneficiary() = default;
    BrokerBeneficiary(const json& j)
        : given_name(parse_string(j, "given_name")),
          middle_name(parse_string(j, "middle_name")),
          family_name(parse_string(j, "family_name")),
          date_of_birth(parse_string(j, "date_of_birth")),
          tax_id(parse_string(j, "tax_id")),
          tax_id_type(tax_id_type_from_string(parse_string(j, "tax_id_type"))),
          relationship(parse_string(j, "relationship")),
          type(parse_string(j, "type")),
          share_pct(parse_string(j, "share_pct")) {}

    json to_json() const {
        json j;
        j["given_name"] = given_name;
        if (!middle_name.empty()) j["middle_name"] = middle_name;
        j["family_name"] = family_name;
        j["date_of_birth"] = date_of_birth;
        if (!tax_id.empty()) j["tax_id"] = tax_id;
        if (tax_id_type != TaxIdType::NOT_SPECIFIED) j["tax_id_type"] = tax_id_type_to_string(tax_id_type);
        j["relationship"] = relationship;
        j["type"] = type;
        j["share_pct"] = share_pct;
        return j;
    }
};

struct BrokerUSD {
    std::string last_equity;

    BrokerUSD() = default;
    BrokerUSD(const json& j)
        : last_equity(parse_string(j, "last_equity")) {}

    json to_json() const {
        json j;
        j["last_equity"] = last_equity;
        return j;
    }
};

struct BrokerCashInterestEntry {
    std::string apr_tier_name;
    std::string status;

    BrokerCashInterestEntry() = default;
    BrokerCashInterestEntry(const json& j)
        : apr_tier_name(parse_string(j, "apr_tier_name")),
          status(parse_string(j, "status")) {}

    json to_json() const {
        json j;
        j["apr_tier_name"] = apr_tier_name;
        j["status"] = status;
        return j;
    }
};

struct BrokerFPSLEntry {
    std::string tier_id;
    std::string status;

    BrokerFPSLEntry() = default;
    BrokerFPSLEntry(const json& j)
        : tier_id(parse_string(j, "tier_id")),
          status(parse_string(j, "status")) {}

    json to_json() const {
        json j;
        j["tier_id"] = tier_id;
        j["status"] = status;
        return j;
    }
};

// ---------------------------------------------------------------------------
// BrokerAccount — full model for GET /v1/accounts responses
// ---------------------------------------------------------------------------

struct BrokerAccount {
    std::string id;
    std::string account_number;
    BrokerAccountStatus status = BrokerAccountStatus::INACTIVE;
    BrokerCryptoStatus crypto_status = BrokerCryptoStatus::INACTIVE;
    KYCResults kyc_results;
    std::string currency;
    std::string last_equity;
    std::string created_at;

    BrokerContact contact;
    BrokerIdentity identity;
    BrokerDisclosures disclosures;
    std::vector<BrokerAgreement> agreements;
    std::vector<BrokerDocument> documents;
    BrokerTrustedContact trusted_contact;

    std::string account_name;
    BrokerAccountType account_type = BrokerAccountType::TRADING;
    std::string account_sub_type;
    std::string custodial_account_type;
    BrokerMinorIdentity minor_identity;
    BrokerTradingConfigurations trading_configurations;

    BrokerUSD usd;
    std::vector<std::string> enabled_assets;
    std::vector<BrokerBeneficiary> beneficiaries;

    // cash_interest: keyed by currency (e.g. "USD")
    std::map<std::string, BrokerCashInterestEntry> cash_interest;
    // fpsl: keyed by market (e.g. "US")
    std::map<std::string, BrokerFPSLEntry> fpsl;

    // Deprecated top-level fields (still present in responses)
    std::string risk_tolerance;
    std::string investment_objective;
    std::string investment_time_horizon;
    std::string liquidity_needs;

    BrokerAccount() = default;
    BrokerAccount(const json& j)
        : id(parse_string(j, "id")),
          account_number(parse_string(j, "account_number")),
          status(broker_account_status_from_string(parse_string(j, "status"))),
          crypto_status(broker_crypto_status_from_string(parse_string(j, "crypto_status"))),
          currency(parse_string(j, "currency")),
          last_equity(parse_string(j, "last_equity")),
          created_at(parse_string(j, "created_at")),
          account_name(parse_string(j, "account_name")),
          account_type(broker_account_type_from_string(parse_string(j, "account_type"))),
          account_sub_type(parse_string(j, "account_sub_type")),
          custodial_account_type(parse_string(j, "custodial_account_type")),
          risk_tolerance(parse_string(j, "risk_tolerance")),
          investment_objective(parse_string(j, "investment_objective")),
          investment_time_horizon(parse_string(j, "investment_time_horizon")),
          liquidity_needs(parse_string(j, "liquidity_needs"))
    {
        if (j.contains("kyc_results") && j["kyc_results"].is_object()) {
            kyc_results = KYCResults(j["kyc_results"]);
        }
        if (j.contains("contact") && j["contact"].is_object()) {
            contact = BrokerContact(j["contact"]);
        }
        if (j.contains("identity") && j["identity"].is_object()) {
            identity = BrokerIdentity(j["identity"]);
        }
        if (j.contains("disclosures") && j["disclosures"].is_object()) {
            disclosures = BrokerDisclosures(j["disclosures"]);
        }
        if (j.contains("agreements") && j["agreements"].is_array()) {
            for (const auto& item : j["agreements"]) {
                agreements.emplace_back(item);
            }
        }
        if (j.contains("documents") && j["documents"].is_array()) {
            for (const auto& item : j["documents"]) {
                documents.emplace_back(item);
            }
        }
        if (j.contains("trusted_contact") && j["trusted_contact"].is_object()) {
            trusted_contact = BrokerTrustedContact(j["trusted_contact"]);
        }
        if (j.contains("minor_identity") && j["minor_identity"].is_object()) {
            minor_identity = BrokerMinorIdentity(j["minor_identity"]);
        }
        if (j.contains("trading_configurations") && j["trading_configurations"].is_object()) {
            trading_configurations = BrokerTradingConfigurations(j["trading_configurations"]);
        }
        if (j.contains("usd") && j["usd"].is_object()) {
            usd = BrokerUSD(j["usd"]);
        }
        if (j.contains("enabled_assets") && j["enabled_assets"].is_array()) {
            enabled_assets = j["enabled_assets"].get<std::vector<std::string>>();
        }
        if (j.contains("beneficiaries") && j["beneficiaries"].is_array()) {
            for (const auto& item : j["beneficiaries"]) {
                beneficiaries.emplace_back(item);
            }
        }
        if (j.contains("cash_interest") && j["cash_interest"].is_object()) {
            for (auto& [key, val] : j["cash_interest"].items()) {
                if (val.is_object()) {
                    cash_interest[key] = BrokerCashInterestEntry(val);
                }
            }
        }
        if (j.contains("fpsl") && j["fpsl"].is_object()) {
            for (auto& [key, val] : j["fpsl"].items()) {
                if (val.is_object()) {
                    fpsl[key] = BrokerFPSLEntry(val);
                }
            }
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "BrokerAccount Information:\n"
            << "  ID: " << id << "\n"
            << "  Account Number: " << account_number << "\n"
            << "  Status: " << broker_account_status_to_string(status) << "\n"
            << "  Crypto Status: " << broker_crypto_status_to_string(crypto_status) << "\n"
            << "  Currency: " << currency << "\n"
            << "  Last Equity: " << last_equity << "\n"
            << "  Created At: " << created_at << "\n"
            << "  Account Type: " << broker_account_type_to_string(account_type) << "\n";
        if (!account_sub_type.empty())
            oss << "  Account Sub Type: " << account_sub_type << "\n";
        if (!account_name.empty())
            oss << "  Account Name: " << account_name << "\n";
        if (!contact.email_address.empty())
            oss << "  Contact Email: " << contact.email_address << "\n";
        if (!identity.given_name.empty())
            oss << "  Identity: " << identity.given_name << " " << identity.family_name << "\n";
        if (!enabled_assets.empty()) {
            oss << "  Enabled Assets: ";
            for (size_t i = 0; i < enabled_assets.size(); ++i) {
                oss << enabled_assets[i];
                if (i + 1 < enabled_assets.size()) oss << ", ";
            }
            oss << "\n";
        }
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["account_number"] = account_number;
        j["status"] = broker_account_status_to_string(status);
        j["crypto_status"] = broker_crypto_status_to_string(crypto_status);
        j["kyc_results"] = kyc_results.to_json();
        j["currency"] = currency;
        j["last_equity"] = last_equity;
        j["created_at"] = created_at;
        j["contact"] = contact.to_json();
        j["identity"] = identity.to_json();
        j["disclosures"] = disclosures.to_json();

        j["agreements"] = json::array();
        for (const auto& a : agreements) j["agreements"].push_back(a.to_json());

        j["documents"] = json::array();
        for (const auto& d : documents) j["documents"].push_back(d.to_json());

        j["trusted_contact"] = trusted_contact.to_json();

        if (!account_name.empty()) j["account_name"] = account_name;
        j["account_type"] = broker_account_type_to_string(account_type);
        if (!account_sub_type.empty()) j["account_sub_type"] = account_sub_type;
        if (!custodial_account_type.empty()) j["custodial_account_type"] = custodial_account_type;

        j["trading_configurations"] = trading_configurations.to_json();
        j["usd"] = usd.to_json();

        if (!enabled_assets.empty()) j["enabled_assets"] = enabled_assets;

        if (!beneficiaries.empty()) {
            j["beneficiaries"] = json::array();
            for (const auto& b : beneficiaries) j["beneficiaries"].push_back(b.to_json());
        }

        if (!cash_interest.empty()) {
            j["cash_interest"] = json::object();
            for (const auto& [key, val] : cash_interest) {
                j["cash_interest"][key] = val.to_json();
            }
        }

        if (!fpsl.empty()) {
            j["fpsl"] = json::object();
            for (const auto& [key, val] : fpsl) {
                j["fpsl"][key] = val.to_json();
            }
        }

        if (!risk_tolerance.empty()) j["risk_tolerance"] = risk_tolerance;
        if (!investment_objective.empty()) j["investment_objective"] = investment_objective;
        if (!investment_time_horizon.empty()) j["investment_time_horizon"] = investment_time_horizon;
        if (!liquidity_needs.empty()) j["liquidity_needs"] = liquidity_needs;

        return j;
    }
};

} // namespace alpaca
