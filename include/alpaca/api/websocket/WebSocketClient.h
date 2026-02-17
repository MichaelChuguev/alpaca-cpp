#pragma once

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "nlohmann/json.hpp"
#include "alpaca/core/types.h"

namespace alpaca {

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
namespace ssl       = net::ssl;
using     tcp       = net::ip::tcp;

/// Low-level, thread-safe WebSocket/SSL client built on top of Boost.Beast.
///
/// Manages the connection lifecycle (connect, authenticate, read loop,
/// reconnect) on a dedicated I/O thread so that the caller's thread is
/// never blocked.
///
/// Subclasses override the virtual hooks to implement protocol-specific
/// authentication, subscription, and message handling.
class WebSocketClient {
public:
    /// Connection state observable by callers.
    enum class State { DISCONNECTED, CONNECTING, CONNECTED, AUTHENTICATED, ERROR };

    using MessageHandler  = std::function<void(const std::string&)>;
    using ErrorHandler    = std::function<void(const std::string&)>;
    using StateHandler    = std::function<void(State)>;

    /// @param host  Host portion of the WSS URL (e.g. "paper-api.alpaca.markets").
    /// @param target  Target path (e.g. "/stream" or "/v2/iex").
    /// @param use_binary  If true the WebSocket frames are binary; text otherwise.
    WebSocketClient(const std::string& host,
                    const std::string& target,
                    bool use_binary = false);

    virtual ~WebSocketClient();

    // ── public API ──────────────────────────────────────────────────────

    /// Open the connection and start the async read-loop.
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
    bool wait_for_state(State target, std::chrono::milliseconds timeout = std::chrono::seconds(10));

    // ── handler registration ────────────────────────────────────────────

    void on_message(MessageHandler handler) { message_handler_ = std::move(handler); }
    void on_error(ErrorHandler handler)     { error_handler_   = std::move(handler); }
    void on_state_change(StateHandler handler) { state_handler_ = std::move(handler); }

    /// Enable / disable automatic reconnection (on by default).
    void set_auto_reconnect(bool enabled, std::chrono::seconds delay = std::chrono::seconds(5));

protected:
    // ── virtual hooks for subclasses ────────────────────────────────────

    /// Called right after the WebSocket handshake succeeds.
    /// Subclasses should send their auth message here.
    virtual void on_connected() {}

    /// Called for every inbound message.  Subclasses process the raw
    /// payload (JSON string) and may choose to call the user-supplied
    /// message_handler_ or keep state internally.
    virtual void on_raw_message(const std::string& payload) {
        if (message_handler_) message_handler_(payload);
    }

    /// Called when the connection drops unexpectedly.
    virtual void on_disconnected(const std::string& reason) {}

    // ── helpers available to subclasses ─────────────────────────────────

    void set_state(State s);
    const std::string& host()   const noexcept { return host_; }
    const std::string& target() const noexcept { return target_; }

private:
    void run_io();
    void do_connect();
    void do_read();
    void do_reconnect();
    void fail(beast::error_code ec, const char* what);

    std::string host_;
    std::string target_;
    bool        use_binary_;

    // ── Boost.Asio / Beast objects ──────────────────────────────────────
    net::io_context                                        ioc_;
    ssl::context                                           ssl_ctx_{ssl::context::tlsv12_client};
    std::unique_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>> ws_;
    tcp::resolver                                          resolver_{ioc_};
    beast::flat_buffer                                     buffer_;

    // ── threading / state ───────────────────────────────────────────────
    std::thread          io_thread_;
    std::atomic<State>   state_{State::DISCONNECTED};
    std::mutex           state_mutex_;
    std::condition_variable state_cv_;

    std::mutex           write_mutex_;

    // ── reconnect ───────────────────────────────────────────────────────
    bool                 auto_reconnect_{true};
    std::chrono::seconds reconnect_delay_{5};
    std::atomic<bool>    shutdown_requested_{false};

    // ── user handlers ───────────────────────────────────────────────────
    MessageHandler message_handler_;
    ErrorHandler   error_handler_;
    StateHandler   state_handler_;
};

} // namespace alpaca
