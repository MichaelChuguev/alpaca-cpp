#pragma once

#include <string>
#include <memory>
#include <boost/beast.hpp>
#include <boost/asio.hpp>


#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class HttpClient {
public:


    //NEW CONSTRUCTORS
    HttpClient(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint) 
    : 
        trader_key_id(traderKeyID),
        trader_key_secret(traderKeySecret),
        trader_api_endpoint(traderAPIEndpoint),
        is_broker_api(false),
        resolver_(std::make_unique<tcp::resolver>(ioc_)) 
    {
    }

    //HttpClient(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint);
    //:
    //{
    //}
    HttpClient(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint)
    : 
        trader_oauth_token(traderOAuthToken),
        trader_api_endpoint(traderAPIEndpoint),
        is_broker_api(false),
        resolver_(std::make_unique<tcp::resolver>(ioc_)) 
{
    // You'd need to decide what to do about trader_key_secret in this case
}

    HttpClient(const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint) 
    : 
        broker_api_key(brokerAPIKey),
        broker_api_secret(brokerAPISecret),
        broker_api_endpoint(brokerAPIEndpoint),
        is_broker_api(true),
        resolver_(std::make_unique<tcp::resolver>(ioc_)) 
    {
    }

    // Core methods
    nlohmann::json get(const std::string& endpoint);
    nlohmann::json post(const std::string& endpoint, const nlohmann::json& body);
    nlohmann::json patch(const std::string& endpoint, const nlohmann::json& body);
    nlohmann::json put(const std::string& endpoint, const nlohmann::json& body);
    nlohmann::json del(const std::string& endpoint);
    
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
    
    const std::string trading_paper = "paper-api.alpaca.markets";
    const std::string trading_live = "api.alpaca.markets";
    const std::string broker_sandbox = "broker-api.sandbox.alpaca.markets";
    const std::string broker_production = "broker-api.alpaca.markets";
    const std::string port = "443";

    net::io_context ioc_;
    std::unique_ptr<tcp::resolver> resolver_;
    
    // Rate limiting
    std::mutex rate_limit_mutex_;
    std::chrono::steady_clock::time_point last_request_;
    
    // Internal helpers
    http::request<http::string_body> build_request(
        http::verb method, 
        const std::string& target,
        const nlohmann::json& body = {}
    );
    
    nlohmann::json do_request(http::request<http::string_body> req);

    std::string get_host() const {
        if (is_broker_api) {
            return broker_api_endpoint == BrokerAPIEndpoint::SANDBOX ? broker_sandbox : broker_production;
        }
        return trader_api_endpoint == TraderAPIEndpoint::PAPER ? trading_paper : trading_live;
    }
};

} // namespace alpaca