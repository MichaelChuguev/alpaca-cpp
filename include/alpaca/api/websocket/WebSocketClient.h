#pragma once

#include <string>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "ixwebsocket/IXWebSocket.h"
#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"

namespace alpaca {

/// Low-level, thread-safe WebSocket/TLS client built on top of IXWebSocket.
///
/// IXWebSocket drives its own internal I/O thread and fires all callbacks on
/// that thread, so the caller's thread is never blocked.  The public surface
/// (connect / disconnect / send / state / wait_for_state / handler slots) is
/// keeping all subclass code and user code unchanged.
///
/// Subclasses override the virtual hooks to implement protocol-specific
/// authentication, subscription, and message handling.
class WebSocketClient {
public:
    /// Connection state observable by callers.
    enum class State { DISCONNECTED, CONNECTING, CONNECTED, AUTHENTICATED, ERROR };

    using MessageHandler = std::function<void(const std::string&)>;
    using ErrorHandler   = std::function<void(const std::string&)>;
    using StateHandler   = std::function<void(State)>;

    /// @param host       Host portion of the WSS URL (e.g. "paper-api.alpaca.markets").
    /// @param target     Target path (e.g. "/stream" or "/v2/iex").
    /// @param use_binary If true frames are sent as binary; text otherwise.
    WebSocketClient(const std::string& host,
                    const std::string& target,
                    bool use_binary = false);

    virtual ~WebSocketClient();

    // ── public API ──────────────────────────────────────────────────────

    /// Open the connection and start the I/O loop.
    void connect();

    /// Gracefully close the connection.
    void disconnect();

    /// Send a JSON message (text or binary depending on constructor flag).
    void send(const nlohmann::json& msg);

    /// Send a raw string message.
    void send_raw(const std::string& msg);

    /// @return Current connection state.
    State state() const noexcept { return state_.load(); }

    /// Block the calling thread until the state becomes @p target or a
    /// timeout is reached.  Returns true if the target state was reached.
    bool wait_for_state(State target,
                        std::chrono::milliseconds timeout = std::chrono::seconds(10));

    // ── handler registration ────────────────────────────────────────────

    void on_message(MessageHandler handler)    { message_handler_ = std::move(handler); }
    void on_error(ErrorHandler handler)        { error_handler_   = std::move(handler); }
    void on_state_change(StateHandler handler) { state_handler_   = std::move(handler); }

    /// Enable / disable automatic reconnection (enabled by default).
    void set_auto_reconnect(bool enabled,
                            std::chrono::seconds delay = std::chrono::seconds(5));

protected:
    // ── virtual hooks for subclasses ────────────────────────────────────

    /// Called immediately after the WebSocket handshake succeeds.
    /// Subclasses should send their authentication message here.
    virtual void on_connected() {}

    /// Called for every inbound message.  The default implementation
    /// forwards to the user-supplied message_handler_ if one is set.
    virtual void on_raw_message(const std::string& payload) {
        if (message_handler_) message_handler_(payload);
    }

    /// Called when the connection drops unexpectedly or is closed by the peer.
    virtual void on_disconnected(const std::string& reason) {}

    // ── helpers available to subclasses ─────────────────────────────────

    void set_state(State s);
    const std::string& host()   const noexcept { return host_; }
    const std::string& target() const noexcept { return target_; }

private:
    void install_callbacks();

    std::string host_;
    std::string target_;
    bool        use_binary_;

    // ── IXWebSocket object (owns its own I/O thread) ─────────────────────
    ix::WebSocket ws_;

    // ── state notification ───────────────────────────────────────────────
    std::atomic<State>      state_{State::DISCONNECTED};
    std::mutex              state_mutex_;
    std::condition_variable state_cv_;

    // ── user handlers ────────────────────────────────────────────────────
    MessageHandler message_handler_;
    ErrorHandler   error_handler_;
    StateHandler   state_handler_;
};

} // namespace alpaca
