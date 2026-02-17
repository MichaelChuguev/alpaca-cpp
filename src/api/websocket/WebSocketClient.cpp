#include "alpaca/api/websocket/WebSocketClient.h"

#include <iostream>
#include <chrono>

namespace alpaca {

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

WebSocketClient::WebSocketClient(const std::string& host,
                                 const std::string& target,
                                 bool use_binary)
    : host_(host), target_(target), use_binary_(use_binary)
{
    ssl_ctx_.set_default_verify_paths();
    ssl_ctx_.set_verify_mode(ssl::verify_peer);
}

WebSocketClient::~WebSocketClient() {
    disconnect();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void WebSocketClient::connect() {
    if (state_ != State::DISCONNECTED && state_ != State::ERROR) return;

    shutdown_requested_ = false;
    set_state(State::CONNECTING);

    // Run the I/O context on a dedicated thread
    io_thread_ = std::thread([this] { run_io(); });
}

void WebSocketClient::disconnect() {
    shutdown_requested_ = true;

    // Close the WebSocket gracefully if it's open
    if (ws_ && ws_->is_open()) {
        beast::error_code ec;
        ws_->close(websocket::close_code::normal, ec);
    }

    // Stop the I/O context
    ioc_.stop();

    if (io_thread_.joinable()) {
        io_thread_.join();
    }

    set_state(State::DISCONNECTED);
}

void WebSocketClient::send(const nlohmann::json& msg) {
    send_raw(msg.dump());
}

void WebSocketClient::send_raw(const std::string& msg) {
    std::lock_guard<std::mutex> lock(write_mutex_);
    if (!ws_ || !ws_->is_open()) return;

    beast::error_code ec;
    ws_->write(net::buffer(msg), ec);
    if (ec) {
        fail(ec, "write");
    }
}

bool WebSocketClient::wait_for_state(State target, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(state_mutex_);
    return state_cv_.wait_for(lock, timeout, [&] {
        return state_.load() == target || state_.load() == State::ERROR;
    });
}

void WebSocketClient::set_auto_reconnect(bool enabled, std::chrono::seconds delay) {
    auto_reconnect_ = enabled;
    reconnect_delay_ = delay;
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void WebSocketClient::set_state(State s) {
    state_.store(s);
    state_cv_.notify_all();
    if (state_handler_) state_handler_(s);
}

void WebSocketClient::run_io() {
    // Reset the io_context in case of a prior run
    ioc_.restart();
    do_connect();
    ioc_.run();
}

void WebSocketClient::do_connect() {
    try {
        // Resolve
        auto const results = resolver_.resolve(host_, "443");

        // Create a new WebSocket stream
        ws_ = std::make_unique<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(ioc_, ssl_ctx_);

        // SNI
        if (!SSL_set_tlsext_host_name(ws_->next_layer().native_handle(), host_.c_str())) {
            throw beast::system_error(
                beast::error_code(static_cast<int>(::ERR_get_error()),
                                  net::error::get_ssl_category()));
        }

        // TCP connect
        auto ep = beast::get_lowest_layer(*ws_).connect(results);

        // Build the Host header (host:port)
        std::string host_header = host_ + ":" + std::to_string(ep.port());

        // SSL handshake
        ws_->next_layer().handshake(ssl::stream_base::client);

        // WebSocket handshake
        ws_->set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req) {
                req.set(boost::beast::http::field::user_agent, "alpaca-cpp/1.0");
            }));

        ws_->handshake(host_header, target_);

        // Configure binary / text mode
        ws_->binary(use_binary_);

        set_state(State::CONNECTED);

        // Let the subclass perform authentication
        on_connected();

        // Enter the read loop
        do_read();

    } catch (const std::exception& e) {
        if (error_handler_) error_handler_(std::string("connect: ") + e.what());
        set_state(State::ERROR);
        do_reconnect();
    }
}

void WebSocketClient::do_read() {
    while (!shutdown_requested_ && ws_ && ws_->is_open()) {
        try {
            buffer_.clear();
            ws_->read(buffer_);

            std::string payload = beast::buffers_to_string(buffer_.data());
            on_raw_message(payload);

        } catch (const beast::system_error& se) {
            if (se.code() == websocket::error::closed) {
                on_disconnected("WebSocket closed normally");
            } else {
                fail(se.code(), "read");
            }
            break;
        } catch (const std::exception& e) {
            if (error_handler_) error_handler_(std::string("read: ") + e.what());
            break;
        }
    }

    if (!shutdown_requested_) {
        on_disconnected("connection lost");
        set_state(State::DISCONNECTED);
        do_reconnect();
    }
}

void WebSocketClient::do_reconnect() {
    if (!auto_reconnect_ || shutdown_requested_) return;

    if (error_handler_) error_handler_("Reconnecting in " + std::to_string(reconnect_delay_.count()) + "s ...");

    std::this_thread::sleep_for(reconnect_delay_);

    if (!shutdown_requested_) {
        set_state(State::CONNECTING);
        do_connect();
    }
}

void WebSocketClient::fail(beast::error_code ec, const char* what) {
    if (error_handler_) {
        error_handler_(std::string(what) + ": " + ec.message());
    }
    set_state(State::ERROR);
}

} // namespace alpaca
