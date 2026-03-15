#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

#include "nlohmann/json.hpp"
#include "ixwebsocket/IXHttpClient.h"

namespace alpaca {

/// Lightweight client for Alpaca Broker SSE streams.
///
/// This class opens an authenticated HTTP connection to a broker events
/// endpoint and parses Server-Sent Events (SSE) frames into Event records.
/// It supports optional auto-reconnect and callback-based delivery for
/// raw event payloads and JSON payloads.
class BrokerSSEClient {
public:
    /// Connection state observable by callers.
    enum class State {
        STOPPED,
        CONNECTING,
        STREAMING,
        ERROR
    };

    /// Parsed SSE event payload.
    struct Event {
        std::string event;
        std::string data;
        std::string id;
        int retry_ms{-1};
    };

    using EventHandler = std::function<void(const Event&)>;
    using JsonHandler = std::function<void(const nlohmann::json&)>;
    using ErrorHandler = std::function<void(const std::string&)>;
    using StateHandler = std::function<void(State)>;

    /// Callable that returns a fresh Bearer token each time it is invoked.
    /// Used by the client-credentials authentication path so that each
    /// reconnect attempt picks up a valid (potentially refreshed) token.
    using TokenProvider = std::function<std::string()>;

    /// Construct from a broker SSE URL and broker API credentials.
    BrokerSSEClient(const std::string& stream_url,
                    const std::string& broker_api_key,
                    const std::string& broker_api_secret);

    /// Construct from a broker SSE URL and a token provider callback.
    /// The callback is invoked before each connection attempt (including
    /// reconnects) to obtain a valid Bearer token — ideal for the
    /// client-credentials flow where tokens expire after 15 minutes.
    BrokerSSEClient(const std::string& stream_url,
                    TokenProvider token_provider);
    ~BrokerSSEClient();

    BrokerSSEClient(const BrokerSSEClient&) = delete;
    BrokerSSEClient& operator=(const BrokerSSEClient&) = delete;

    // ── high-level API ──────────────────────────────────────────────────

    /// Start the stream loop on a background thread.
    void start();

    /// Request shutdown and block until the worker thread exits.
    void stop();

    bool is_running() const noexcept { return running_.load(); }
    State state() const noexcept { return state_.load(); }

    /// Enable / disable reconnect on disconnect or stream failure.
    void set_auto_reconnect(bool enabled,
                            std::chrono::milliseconds delay = std::chrono::seconds(2));

    // ── callbacks ───────────────────────────────────────────────────────

    void on_event(EventHandler handler);
    void on_json(JsonHandler handler);
    void on_error(ErrorHandler handler);
    void on_state_change(StateHandler handler);

private:
    void run();
    void parse_chunk(const std::string& chunk);
    void parse_line(const std::string& line);
    void dispatch_event();
    void clear_pending_event();

    void set_state(State next_state);
    void emit_error(const std::string& message);

    void sleep_reconnect_delay() const;

    std::string stream_url_;
    std::string broker_api_key_;
    std::string broker_api_secret_;
    TokenProvider token_provider_;
    bool use_token_provider_{false};

    ix::HttpClient http_client_;

    std::atomic<bool> running_{false};
    std::atomic<bool> stop_requested_{false};
    std::atomic<bool> has_received_stream_data_{false};
    std::atomic<State> state_{State::STOPPED};

    std::atomic<bool> auto_reconnect_{true};
    std::atomic<int> reconnect_delay_ms_{2000};

    std::mutex request_mutex_;
    ix::HttpRequestArgsPtr active_request_;

    std::thread worker_thread_;

    std::string line_buffer_;
    std::string pending_event_type_;
    std::string pending_event_id_;
    std::string pending_data_;
    int pending_retry_ms_{-1};

    mutable std::mutex callback_mutex_;
    EventHandler event_handler_;
    JsonHandler json_handler_;
    ErrorHandler error_handler_;
    StateHandler state_handler_;
};

} // namespace alpaca
