#include <thread>
#include <stdexcept>

#include "alpaca/api/rest/HTTPClient.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

static void apply_tls(ix::HttpClient& client) {
    ix::SocketTLSOptions tls;
    tls.caFile = "SYSTEM";  // use the OS trust store
    client.setTLSOptions(tls);
}

HttpClient::HttpClient(const std::string& traderKeyID,
                       const std::string& traderKeySecret,
                       const TraderAPIEndpoint& traderAPIEndpoint)
    : trader_key_id(traderKeyID)
    , trader_key_secret(traderKeySecret)
    , trader_api_endpoint(traderAPIEndpoint)
    , is_broker_api(false)
    , is_market_data_api(false)
    , market_data_endpoint(MarketDataEndpoint::SANDBOX)
{
    apply_tls(http_client_);
}

HttpClient::HttpClient(const std::string& traderOAuthToken,
                       const TraderAPIEndpoint& traderAPIEndpoint)
    : trader_oauth_token(traderOAuthToken)
    , trader_api_endpoint(traderAPIEndpoint)
    , is_broker_api(false)
    , is_market_data_api(false)
    , market_data_endpoint(MarketDataEndpoint::SANDBOX)
{
    apply_tls(http_client_);
}

HttpClient::HttpClient(const std::string& brokerAPIKey,
                       const std::string& brokerAPISecret,
                       const BrokerAPIEndpoint& brokerAPIEndpoint)
    : broker_api_key(brokerAPIKey)
    , broker_api_secret(brokerAPISecret)
    , broker_api_endpoint(brokerAPIEndpoint)
    , is_broker_api(true)
    , is_market_data_api(false)
    , market_data_endpoint(MarketDataEndpoint::SANDBOX)
{
    apply_tls(http_client_);
}

HttpClient::HttpClient(const std::string& keyID,
                       const std::string& keySecret,
                       const MarketDataEndpoint& marketDataEndpoint)
    : trader_key_id(keyID)
    , trader_key_secret(keySecret)
    , is_broker_api(false)
    , is_market_data_api(true)
    , market_data_endpoint(marketDataEndpoint)
{
    apply_tls(http_client_);
}

// ---------------------------------------------------------------------------
// Public HTTP verbs
// ---------------------------------------------------------------------------

nlohmann::json HttpClient::get(const std::string& endpoint) {
    return do_request("GET", endpoint);
}

nlohmann::json HttpClient::post(const std::string& endpoint, const nlohmann::json& body) {
    return do_request("POST", endpoint, body);
}

nlohmann::json HttpClient::patch(const std::string& endpoint, const nlohmann::json& body) {
    return do_request("PATCH", endpoint, body);
}

nlohmann::json HttpClient::put(const std::string& endpoint, const nlohmann::json& body) {
    return do_request("PUT", endpoint, body);
}

nlohmann::json HttpClient::del(const std::string& endpoint) {
    return do_request("DELETE", endpoint);
}

// ---------------------------------------------------------------------------
// Async variant
// ---------------------------------------------------------------------------

void HttpClient::async_get(const std::string& endpoint,
                           std::function<void(nlohmann::json)> callback) {
    std::thread([this, endpoint, cb = std::move(callback)]() {
        try {
            cb(get(endpoint));
        } catch (...) {
            cb(nlohmann::json::object());
        }
    }).detach();
}

// ---------------------------------------------------------------------------
// Build per-request arguments (headers, timeouts, TLS)
// ---------------------------------------------------------------------------

ix::HttpRequestArgsPtr HttpClient::make_args() const {
    auto args = std::make_shared<ix::HttpRequestArgs>();
    args->connectTimeout  = 30;
    args->transferTimeout = 60;

    args->extraHeaders["User-Agent"] = "alpaca-cpp/1.0";
    args->extraHeaders["Accept"]     = "application/json";

    if (is_broker_api) {
        args->extraHeaders["APCA-API-KEY-ID"]     = broker_api_key;
        args->extraHeaders["APCA-API-SECRET-KEY"] = broker_api_secret;
    } else if (!trader_oauth_token.empty()) {
        args->extraHeaders["Authorization"] = "Bearer " + trader_oauth_token;
    } else {
        args->extraHeaders["APCA-API-KEY-ID"]     = trader_key_id;
        args->extraHeaders["APCA-API-SECRET-KEY"] = trader_key_secret;
    }

    return args;
}

// ---------------------------------------------------------------------------
// Core request executor
// ---------------------------------------------------------------------------

nlohmann::json HttpClient::do_request(const std::string& method,
                                      const std::string& endpoint,
                                      const nlohmann::json& body) {
    // Simple rate-limit: enforce ≥300 ms between requests.
    {
        std::lock_guard lock(rate_limit_mutex_);
        auto now     = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - last_request_).count();
        if (elapsed < 300) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300 - elapsed));
        }
        last_request_ = std::chrono::steady_clock::now();
    }

    const std::string url  = build_url(endpoint);
    auto              args = make_args();

    ix::HttpResponsePtr response;

    if (method == "GET") {
        response = http_client_.get(url, args);
    } else if (method == "DELETE") {
        response = http_client_.Delete(url, args);
    } else {
        std::string body_str;
        if (!body.empty()) {
            body_str = body.dump();
            args->extraHeaders["Content-Type"] = "application/json";
        }
        if (method == "POST") {
            response = http_client_.post(url, body_str, args);
        } else if (method == "PATCH") {
            response = http_client_.patch(url, body_str, args);
        } else if (method == "PUT") {
            response = http_client_.put(url, body_str, args);
        } else {
            throw std::runtime_error("Unknown HTTP method: " + method);
        }
    }

    if (!response) {
        throw std::runtime_error("HTTP request failed: null response");
    }

    if (response->errorCode != ix::HttpErrorCode::Ok) {
        throw std::runtime_error("HTTP transport error: " + response->errorMsg);
    }

    if (response->statusCode != 200 &&
        response->statusCode != 201 &&
        response->statusCode != 204) {
        throw std::runtime_error(
            "HTTP " + std::to_string(response->statusCode) + ": " + response->body);
    }

    if (response->body.empty()) {
        return nlohmann::json::object();
    }

    return nlohmann::json::parse(response->body);
}

} // namespace alpaca