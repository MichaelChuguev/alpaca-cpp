#include "alpaca/api/rest/auth/AlpacaAuthAPI.h"

namespace alpaca {

namespace {
constexpr const char* kClientCredentialsGrantType = "client_credentials";
constexpr const char* kJwtBearerAssertionType =
    "urn:ietf:params:oauth:client-assertion-type:jwt-bearer";
} // namespace

// ---------------------------------------------------------------------------
// OAuth2 token exchange
// ---------------------------------------------------------------------------

AlpacaAuthAPI::TokenResponse AlpacaAuthAPI::issue_tokens(
    const std::string& grant_type,
    const std::string& client_assertion_type,
    const std::string& client_assertion) {

    if (client_id.empty()) {
        throw std::invalid_argument("client_id is required.");
    }

    if (grant_type != kClientCredentialsGrantType) {
        throw std::invalid_argument("Unsupported grant_type. Use client_credentials.");
    }

    std::vector<std::pair<std::string, std::string>> fields;
    fields.emplace_back("grant_type", grant_type);
    fields.emplace_back("client_id", client_id);

    if (!client_assertion.empty()) {
        if (client_assertion_type.empty()) {
            throw std::invalid_argument("client_assertion_type is required when client_assertion is provided.");
        }

        if (client_assertion_type != kJwtBearerAssertionType) {
            throw std::invalid_argument(
                "Unsupported client_assertion_type. Use urn:ietf:params:oauth:client-assertion-type:jwt-bearer.");
        }

        fields.emplace_back("client_assertion_type", client_assertion_type);
        fields.emplace_back("client_assertion", client_assertion);
    } else {
        if (!client_assertion_type.empty()) {
            throw std::invalid_argument("client_assertion is required when client_assertion_type is provided.");
        }

        if (client_secret.empty()) {
            throw std::invalid_argument("client_secret is required when client_assertion is not provided.");
        }

        fields.emplace_back("client_secret", client_secret);
    }

    // Auth API host already includes /v1 base path in HttpClient.
    return TokenResponse(httpClient.post_form("/oauth2/token", fields));
}

} // namespace alpaca
