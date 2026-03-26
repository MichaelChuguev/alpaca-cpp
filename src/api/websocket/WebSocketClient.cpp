#include "alpaca/api/websocket/WebSocketClient.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

WebSocketClient::WebSocketClient(const std::string& host,
                                 const std::string& target,
                                 bool use_binary)
    : host_(host), target_(target), use_binary_(use_binary)
{
    // IXWebSocket infers TLS from the "wss://" scheme automatically.
    // Enforce verified TLS (mbedTLS or OpenSSL backend, whichever was compiled in).
    ix::SocketTLSOptions tls;
    tls.caFile = "SYSTEM"; // use the system trust store
    ws_.setTLSOptions(tls);

    install_callbacks();
}

WebSocketClient::~WebSocketClient() {
    disconnect();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void WebSocketClient::connect() {
    if (state_ != State::DISCONNECTED && state_ != State::ERROR) return;

    set_state(State::CONNECTING);
    ws_.setExtraHeaders(extra_headers_);
    ws_.setUrl("wss://" + host_ + target_);
    ws_.start();
}

void WebSocketClient::disconnect() {
    ws_.stop();
    set_state(State::DISCONNECTED);
}

void WebSocketClient::send(const nlohmann::json& msg) {
    send_raw(msg.dump());
}

void WebSocketClient::send_msgpack(const nlohmann::json& msg) {
    const auto packed = nlohmann::json::to_msgpack(msg);
    send_raw(std::string(packed.begin(), packed.end()));
}

void WebSocketClient::send_raw(const std::string& msg) {
    // ix::WebSocket::send() / sendBinary() are thread-safe.
    if (use_binary_) {
        ws_.sendBinary(msg);
    } else {
        ws_.send(msg);
    }
}

void WebSocketClient::set_extra_headers(const std::map<std::string, std::string>& headers) {
    extra_headers_ = ix::WebSocketHttpHeaders(headers.begin(), headers.end());
    ws_.setExtraHeaders(extra_headers_);
}

bool WebSocketClient::wait_for_state(State target, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(state_mutex_);
    return state_cv_.wait_for(lock, timeout, [&] {
        return state_.load() == target || state_.load() == State::ERROR;
    });
}

void WebSocketClient::set_auto_reconnect(bool enabled, std::chrono::seconds delay) {
    if (enabled) {
        ws_.enableAutomaticReconnection();
        uint32_t ms = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(delay).count());
        ws_.setMinWaitBetweenReconnectionRetries(ms);
        ws_.setMaxWaitBetweenReconnectionRetries(ms * 4); // 4× exponential cap
    } else {
        ws_.disableAutomaticReconnection();
    }
}

// ---------------------------------------------------------------------------
// Callback installation (called once from constructor)
// ---------------------------------------------------------------------------

void WebSocketClient::install_callbacks() {
    ws_.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        using T = ix::WebSocketMessageType;

        switch (msg->type) {
        case T::Open:
            // Handshake succeeded — tell subclass to send its auth message.
            set_state(State::CONNECTED);
            on_connected();
            break;

        case T::Message:
            // Hot path: inbound data frame.
            on_raw_message(msg->str);
            break;

        case T::Close:
            on_disconnected(msg->closeInfo.reason);
            // IXWebSocket will reconnect on its own if enabled; we just
            // reflect the transient state so callers can observe it.
            if (state_.load() != State::DISCONNECTED) {
                set_state(State::DISCONNECTED);
            }
            break;

        case T::Error:
            if (error_handler_) {
                error_handler_(msg->errorInfo.reason);
            }
            set_state(State::ERROR);
            break;

        default:
            break;
        }
    });
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void WebSocketClient::set_state(State s) {
    state_.store(s);
    state_cv_.notify_all();
    if (state_handler_) state_handler_(s);
}

} // namespace alpaca
