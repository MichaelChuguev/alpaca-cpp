#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <utility>

#include "ixwebsocket/IXHttpClient.h"
#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"

namespace alpaca {

class HttpClient {
public:

    // Non-copyable, non-movable (owns HTTP client and mutex)
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
    HttpClient(HttpClient&&) = delete;
    HttpClient& operator=(HttpClient&&) = delete;

    HttpClient(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint);
    HttpClient(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint);
    HttpClient(const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint);

    // Client-credentials auth for Broker API: obtains/refreshes a Bearer token
    // automatically.  Pass BrokerAuthMode::CLIENT_CREDENTIALS to activate.
    HttpClient(const std::string& clientId, const std::string& clientSecret,
               const BrokerAPIEndpoint& brokerAPIEndpoint, BrokerAuthMode authMode);
    HttpClient(const std::string& keyID, const std::string& keySecret, const MarketDataEndpoint& marketDataEndpoint);
    HttpClient(const std::string& clientID, const std::string& clientSecret, const AuthAPIEndpoint& authAPIEndpoint);

    // Core methods
    virtual nlohmann::json get(const std::string& endpoint);
    virtual nlohmann::json post(const std::string& endpoint, const nlohmann::json& body);
    virtual nlohmann::json post_form(const std::string& endpoint, const std::vector<std::pair<std::string, std::string>>& fields);
    virtual nlohmann::json patch(const std::string& endpoint, const nlohmann::json& body);
    virtual nlohmann::json put(const std::string& endpoint, const nlohmann::json& body);
    virtual nlohmann::json del(const std::string& endpoint);

    virtual ~HttpClient() = default;

    /** Returns the full base URL (e.g. "https://data.alpaca.markets") for this client. */
    std::string get_base_url() const { return "https://" + get_host(); }

    /**
     * Returns a valid Broker API Bearer token, refreshing it first if it has
     * expired or is about to expire.  Only meaningful when the client was
     * constructed with BrokerAuthMode::CLIENT_CREDENTIALS; throws otherwise.
     */
    std::string get_broker_bearer_token() const;

    // Async variants
    void async_get(const std::string& endpoint, std::function<void(nlohmann::json)> callback);

private:
    // Member variables

    std::string client_id;
    std::string client_secret;
    AuthAPIEndpoint auth_api_endpoint;

    std::string trader_key_id;
    std::string trader_key_secret;
    std::string trader_oauth_token;
    TraderAPIEndpoint trader_api_endpoint;

    std::string broker_api_key;
    std::string broker_api_secret;
    BrokerAPIEndpoint broker_api_endpoint;

    bool is_auth_api{false};
    bool is_broker_api{false};
    bool is_market_data_api{false};
    bool broker_use_client_credentials{false};

    // Client-credentials token cache (mutable so const helpers can refresh).
    std::string broker_auth_token_url_;
    mutable std::mutex token_mutex_;
    mutable std::string broker_access_token_;
    mutable std::chrono::steady_clock::time_point broker_token_expiry_;

    MarketDataEndpoint market_data_endpoint;

    const std::string auth_sandbox       = "authx.sandbox.alpaca.markets";
    const std::string auth_production    = "authx.alpaca.markets";
    const std::string auth_base_path     = "/v1";
    const std::string trading_paper      = "paper-api.alpaca.markets";
    const std::string trading_live       = "api.alpaca.markets";
    const std::string broker_sandbox     = "broker-api.sandbox.alpaca.markets";
    const std::string broker_production  = "broker-api.alpaca.markets";
    const std::string data_sandbox       = "data.sandbox.alpaca.markets";
    const std::string data_live          = "data.alpaca.markets";

    ix::HttpClient http_client_;

    // Fetch/refresh the broker OAuth2 Bearer token when needed.
    void ensure_broker_token() const;

    // Rate limiting
    std::mutex rate_limit_mutex_;
    std::chrono::steady_clock::time_point last_request_;

    // Internal helpers
    ix::HttpRequestArgsPtr make_args() const;
    nlohmann::json do_request(const std::string& method,
                              const std::string& endpoint,
                              const nlohmann::json& body = {});

    std::string get_host() const {
        if (is_auth_api) {
            return auth_api_endpoint == AuthAPIEndpoint::SANDBOX ? auth_sandbox : auth_production;
        }
        if (is_market_data_api) {
            return market_data_endpoint == MarketDataEndpoint::SANDBOX ? data_sandbox : data_live;
        }
        if (is_broker_api) {
            return broker_api_endpoint == BrokerAPIEndpoint::SANDBOX ? broker_sandbox : broker_production;
        }
        return trader_api_endpoint == TraderAPIEndpoint::PAPER ? trading_paper : trading_live;
    }

    std::string build_url(const std::string& endpoint) const {
        if (is_auth_api) {
            if (endpoint.empty()) {
                return "https://" + get_host() + auth_base_path;
            }

            std::string normalized = endpoint;
            if (normalized.front() != '/') {
                normalized = "/" + normalized;
            }
            if (normalized == auth_base_path || normalized.rfind(auth_base_path + "/", 0) == 0) {
                return "https://" + get_host() + normalized;
            }
            return "https://" + get_host() + auth_base_path + normalized;
        }
        return "https://" + get_host() + endpoint;
    }
};

} // namespace alpaca