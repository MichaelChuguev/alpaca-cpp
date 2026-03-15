#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "nlohmann/json.hpp"

#include "alpaca/api/rest/HTTPClient.h"

namespace alpaca {

using json = nlohmann::json;

class AlpacaAuthAPI {

public:
    struct TokenResponse {
        std::string access_token;
        int expires_in = 0;
        std::string token_type;

        TokenResponse() = default;

        explicit TokenResponse(const json& j)
            : access_token(j.value("access_token", ""))
            , expires_in(j.value("expires_in", 0))
            , token_type(j.value("token_type", "")) {}
    };

protected:
   
    std::string client_id;
    std::string client_secret;
    AuthAPIEndpoint authAPIEndpoint;

    HttpClient httpClient;

public:

    /**
     * Instantiates a new AlpacaAuthAPI.
     *
     * @param client_id         
     * @param client_secret     
     * @param authAPIEndpoint  
     * 
     */
    AlpacaAuthAPI(const std::string& client_id, const std::string& client_secret, const AuthAPIEndpoint& authAPIEndpoint) 
    :
        client_id(client_id),
        client_secret(client_secret),
        authAPIEndpoint(authAPIEndpoint),
        httpClient(client_id, client_secret, authAPIEndpoint)
    { 
    }

    /**
     * Exchange client credentials for an access token.
     *
     * The Authentication API currently supports grant_type=client_credentials.
     * If client_assertion is provided, client_assertion_type is required and
     * client_secret will not be sent.
     */
    TokenResponse issue_tokens(
        const std::string& grant_type = "client_credentials",
        const std::string& client_assertion_type = "",
        const std::string& client_assertion = "");

};

}