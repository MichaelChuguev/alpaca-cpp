#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>

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
    HttpClient(const std::string& keyID, const std::string& keySecret, const MarketDataEndpoint& marketDataEndpoint);

    // Core methods
    virtual nlohmann::json get(const std::string& endpoint);
    virtual nlohmann::json post(const std::string& endpoint, const nlohmann::json& body);
    virtual nlohmann::json patch(const std::string& endpoint, const nlohmann::json& body);
    virtual nlohmann::json put(const std::string& endpoint, const nlohmann::json& body);
    virtual nlohmann::json del(const std::string& endpoint);

    virtual ~HttpClient() = default;

    /** Returns the full base URL (e.g. "https://data.alpaca.markets") for this client. */
    std::string get_base_url() const { return "https://" + get_host(); }

    // Async variants
    void async_get(const std::string& endpoint, std::function<void(nlohmann::json)> callback);

private:
    // Member variables
    std::string trader_key_id;
    std::string trader_key_secret;
    std::string trader_oauth_token;
    TraderAPIEndpoint trader_api_endpoint;

    std::string broker_api_key;
    std::string broker_api_secret;
    BrokerAPIEndpoint broker_api_endpoint;

    bool is_broker_api;
    bool is_market_data_api;

    MarketDataEndpoint market_data_endpoint;

    const std::string trading_paper      = "paper-api.alpaca.markets";
    const std::string trading_live       = "api.alpaca.markets";
    const std::string broker_sandbox     = "broker-api.sandbox.alpaca.markets";
    const std::string broker_production  = "broker-api.alpaca.markets";
    const std::string data_sandbox       = "data.sandbox.alpaca.markets";
    const std::string data_live          = "data.alpaca.markets";

    ix::HttpClient http_client_;

    // Rate limiting
    std::mutex rate_limit_mutex_;
    std::chrono::steady_clock::time_point last_request_;

    // Internal helpers
    ix::HttpRequestArgsPtr make_args() const;
    nlohmann::json do_request(const std::string& method,
                              const std::string& endpoint,
                              const nlohmann::json& body = {});

    std::string get_host() const {
        if (is_market_data_api) {
            return market_data_endpoint == MarketDataEndpoint::SANDBOX ? data_sandbox : data_live;
        }
        if (is_broker_api) {
            return broker_api_endpoint == BrokerAPIEndpoint::SANDBOX ? broker_sandbox : broker_production;
        }
        return trader_api_endpoint == TraderAPIEndpoint::PAPER ? trading_paper : trading_live;
    }

    std::string build_url(const std::string& endpoint) const {
        return "https://" + get_host() + endpoint;
    }
};

} // namespace alpaca