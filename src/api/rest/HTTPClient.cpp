#include <iostream>
#include <exception>
#include <boost/asio/ssl.hpp>

#include "alpaca/api/rest/HTTPClient.h"

namespace alpaca {

namespace ssl = boost::asio::ssl;

nlohmann::json HttpClient::get(const std::string& endpoint) {
    auto req = build_request(http::verb::get, endpoint);
    return do_request(std::move(req));
}

nlohmann::json HttpClient::post(const std::string& endpoint, const nlohmann::json& body) {
    auto req = build_request(http::verb::post, endpoint, body);
    return do_request(std::move(req));
}

nlohmann::json HttpClient::patch(const std::string& endpoint, const nlohmann::json& body) {
    auto req = build_request(http::verb::patch, endpoint, body);
    return do_request(std::move(req));
}

nlohmann::json HttpClient::put(const std::string& endpoint, const nlohmann::json& body) {
    auto req = build_request(http::verb::put, endpoint, body);
    return do_request(std::move(req));
}

nlohmann::json HttpClient::del(const std::string& endpoint) {
    auto req = build_request(http::verb::delete_, endpoint);
    return do_request(std::move(req));
}

http::request<http::string_body> HttpClient::build_request(http::verb method, const std::string& target,
    const nlohmann::json& body
) {
    {
        std::lock_guard lock(rate_limit_mutex_);
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_request_).count();
        if (elapsed < 300) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300 - elapsed));
        }
        last_request_ = now;
    }

    http::request<http::string_body> req{method, target, 11};
    req.set(http::field::host, get_host());
    req.set(http::field::user_agent, "alpaca-cpp/1.0");
    req.set(http::field::accept, "application/json");

    // Set authentication headers
    if (is_broker_api) {
        //TODO CHANGE FOR BROKER
        req.set("APCA-API-KEY-ID", broker_api_key);
        req.set("APCA-API-SECRET-KEY", broker_api_secret);
    } else {
        req.set("APCA-API-KEY-ID", trader_key_id);
        req.set("APCA-API-SECRET-KEY", trader_key_secret);
    }

    if (!body.empty()) {
        req.set(http::field::content_type, "application/json");
        req.body() = body.dump();
        req.prepare_payload();
    }

    return req;
}

nlohmann::json HttpClient::do_request(http::request<http::string_body> req) {
    ssl::context ctx{ssl::context::tlsv12_client};
    
    ctx.set_default_verify_paths();
    ctx.set_verify_mode(ssl::verify_peer);

    auto const results = resolver_->resolve(get_host(), port);

    // SSL stream
    ssl::stream<beast::tcp_stream> stream{ioc_, ctx};
    if (!SSL_set_tlsext_host_name(stream.native_handle(), get_host().c_str())) {
        throw beast::system_error(
            beast::error_code(static_cast<int>(::ERR_get_error()),
            net::error::get_ssl_category()));
    }

    // Connect and handshake
    beast::get_lowest_layer(stream).connect(results);
    stream.handshake(ssl::stream_base::client);

    // Send request
    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response_parser<http::dynamic_body> parser;
    parser.body_limit(1024ULL * 1024 * 1024);  // 1GB limit

    // Read the response using the parser with the new limit
    http::read(stream, buffer, parser);
    auto res = parser.get();

    // Graceful shutdown
    beast::error_code ec;
    stream.shutdown(ec);

    // Parse the JSON from the response body
    return nlohmann::json::parse(beast::buffers_to_string(res.body().data()));
}



} // namespace alpaca