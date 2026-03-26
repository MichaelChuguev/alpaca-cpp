#include <thread>
#include <stdexcept>
#include <sstream>

#include "alpaca/api/rest/HTTPClient.h"
#include "alpaca/core/error.h"
#include "alpaca/core/util.h"

namespace alpaca {

namespace {

[[noreturn]] void throw_http_status_error(int status_code, const std::string& response_body) {
    throw AlpacaException::from_http_response(status_code, response_body);
}

[[noreturn]] void throw_transport_error(const std::string& message,
                                        const std::string& response_body = {}) {
    throw AlpacaException::from_transport_error(message, response_body);
}

} // namespace

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
    , is_auth_api(false)
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
    , is_auth_api(false)
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
    , is_auth_api(false)
    , is_broker_api(true)
    , is_market_data_api(false)
    , broker_use_client_credentials(false)
    , market_data_endpoint(MarketDataEndpoint::SANDBOX)
{
    apply_tls(http_client_);
}

HttpClient::HttpClient(const std::string& clientId,
                       const std::string& clientSecret,
                       const BrokerAPIEndpoint& brokerAPIEndpoint,
                       BrokerAuthMode authMode)
    : broker_api_key(clientId)
    , broker_api_secret(clientSecret)
    , broker_api_endpoint(brokerAPIEndpoint)
    , is_auth_api(false)
    , is_broker_api(true)
    , is_market_data_api(false)
    , broker_use_client_credentials(authMode == BrokerAuthMode::CLIENT_CREDENTIALS)
    , broker_auth_token_url_(
          brokerAPIEndpoint == BrokerAPIEndpoint::SANDBOX
              ? "https://authx.sandbox.alpaca.markets/v1/oauth2/token"
              : "https://authx.alpaca.markets/v1/oauth2/token")
    , market_data_endpoint(MarketDataEndpoint::SANDBOX)
{
    apply_tls(http_client_);
}

HttpClient::HttpClient(const std::string& keyID,
                       const std::string& keySecret,
                       const MarketDataEndpoint& marketDataEndpoint)
    : trader_key_id(keyID)
    , trader_key_secret(keySecret)
    , is_auth_api(false)
    , is_broker_api(false)
    , is_market_data_api(true)
    , market_data_endpoint(marketDataEndpoint)
{
    apply_tls(http_client_);
}

HttpClient::HttpClient(const std::string& clientID,
                       const std::string& clientSecret,
                       const AuthAPIEndpoint& authAPIEndpoint)
    : client_id(clientID)
    , client_secret(clientSecret)
    , auth_api_endpoint(authAPIEndpoint)
    , trader_api_endpoint(TraderAPIEndpoint::PAPER)
    , broker_api_endpoint(BrokerAPIEndpoint::SANDBOX)
    , is_auth_api(true)
    , is_broker_api(false)
    , is_market_data_api(false)
    , market_data_endpoint(MarketDataEndpoint::SANDBOX)
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

nlohmann::json HttpClient::post_form(const std::string& endpoint,
                                     const std::vector<std::pair<std::string, std::string>>& fields) {
    std::ostringstream body_stream;
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            body_stream << "&";
        }
        body_stream << url_encode(fields[i].first) << "=" << url_encode(fields[i].second);
    }

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

    auto args = make_args();
    args->extraHeaders["Content-Type"] = "application/x-www-form-urlencoded";

    const auto response = http_client_.post(build_url(endpoint), body_stream.str(), args);

    if (!response) {
        throw_transport_error("HTTP request failed: null response");
    }

    if (response->errorCode != ix::HttpErrorCode::Ok) {
        throw_transport_error("HTTP transport error: " + response->errorMsg, response->body);
    }

    if (response->statusCode != 200 &&
        response->statusCode != 201 &&
        response->statusCode != 204) {
        throw_http_status_error(response->statusCode, response->body);
    }

    if (response->body.empty()) {
        return nlohmann::json::object();
    }

    return nlohmann::json::parse(response->body);
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
// Broker client-credentials token management
// ---------------------------------------------------------------------------

void HttpClient::ensure_broker_token() const {
    std::lock_guard<std::mutex> lock(token_mutex_);

    const auto now = std::chrono::steady_clock::now();
    // Reuse an existing token if it is still valid for more than 60 seconds.
    if (!broker_access_token_.empty() &&
        (broker_token_expiry_ - now) > std::chrono::seconds(60)) {
        return;
    }

    // Build the request body as application/x-www-form-urlencoded.
    const std::string body =
        "grant_type=client_credentials"
        "&client_id="     + url_encode(broker_api_key) +
        "&client_secret=" + url_encode(broker_api_secret);

    ix::HttpClient token_client(false);
    ix::SocketTLSOptions tls;
    tls.caFile = "SYSTEM";
    token_client.setTLSOptions(tls);

    auto args = std::make_shared<ix::HttpRequestArgs>();
    args->connectTimeout  = 30;
    args->transferTimeout = 60;
    args->extraHeaders["Content-Type"] = "application/x-www-form-urlencoded";
    args->extraHeaders["User-Agent"]   = "alpaca-cpp/1.0";
    args->extraHeaders["Accept"]       = "application/json";

    const auto response = token_client.post(broker_auth_token_url_, body, args);

    if (!response || response->errorCode != ix::HttpErrorCode::Ok) {
        throw_transport_error(
            "Broker token refresh failed: " +
            (response ? response->errorMsg : "null response"),
            response ? response->body : "");
    }

    if (response->statusCode != 200) {
        throw_http_status_error(response->statusCode, response->body);
    }

    const auto j        = nlohmann::json::parse(response->body);
    broker_access_token_ = j.at("access_token").get<std::string>();
    const int expires_in = j.at("expires_in").get<int>();
    broker_token_expiry_ = std::chrono::steady_clock::now() +
                           std::chrono::seconds(expires_in);
}

std::string HttpClient::get_broker_bearer_token() const {
    if (!broker_use_client_credentials) {
        throw std::runtime_error(
            "get_broker_bearer_token() called on a non-client-credentials HttpClient");
    }
    ensure_broker_token();
    std::lock_guard<std::mutex> lock(token_mutex_);
    return broker_access_token_;
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

    if (is_auth_api) {
        return args;
    }

    if (is_broker_api) {
        if (broker_use_client_credentials) {
            // Obtain/refresh the OAuth2 Bearer token and attach it.
            ensure_broker_token();
            std::lock_guard<std::mutex> tok_lock(token_mutex_);
            args->extraHeaders["Authorization"] = "Bearer " + broker_access_token_;
        } else {
            args->extraHeaders["APCA-API-KEY-ID"]     = broker_api_key;
            args->extraHeaders["APCA-API-SECRET-KEY"] = broker_api_secret;
        }
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
        throw_transport_error("HTTP request failed: null response");
    }

    if (response->errorCode != ix::HttpErrorCode::Ok) {
        throw_transport_error("HTTP transport error: " + response->errorMsg, response->body);
    }

    if (response->statusCode != 200 &&
        response->statusCode != 201 &&
        response->statusCode != 204) {
        throw_http_status_error(response->statusCode, response->body);
    }

    if (response->body.empty()) {
        return nlohmann::json::object();
    }

    return nlohmann::json::parse(response->body);
}

} // namespace alpaca
