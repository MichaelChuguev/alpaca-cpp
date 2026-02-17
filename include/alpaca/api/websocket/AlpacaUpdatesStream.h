#pragma once

#include <string>
#include <functional>
#include <vector>
#include <atomic>

#include "alpaca/api/websocket/WebSocketClient.h"
#include "alpaca/model/stream/trade_update.h"
#include "alpaca/core/types.h"

namespace alpaca {

/// WebSocket stream for trade / order updates.
///
/// Connects to:
///   Paper:  wss://paper-api.alpaca.markets/stream
///   Live:   wss://api.alpaca.markets/stream
///
/// **Uses binary WebSocket frames** (Alpaca sends binary on this endpoint).
///
/// Protocol:
///   1. Send auth:   {"action":"auth","key":"...","secret":"..."}
///   2. Receive:     {"stream":"authorization","data":{"status":"authorized",...}}
///   3. Send listen: {"action":"listen","data":{"streams":["trade_updates"]}}
///   4. Receive:     {"stream":"listening","data":{"streams":["trade_updates"]}}
///   5. Receive updates: {"stream":"trade_updates","data":{...}}
class AlpacaUpdatesStream : public WebSocketClient {
public:
    using TradeUpdateHandler = std::function<void(const TradeUpdate&)>;
    using AuthHandler        = std::function<void(bool /*authorized*/)>;

    /// Construct with API credentials and endpoint.
    AlpacaUpdatesStream(const std::string& key_id,
                        const std::string& secret_key,
                        TraderAPIEndpoint  endpoint = PAPER);

    // ── high-level API ──────────────────────────────────────────────────

    /// Start listening for trade_updates (connects + authenticates + subscribes).
    void start();

    /// Stop listening and disconnect.
    void stop();

    /// Add / update the set of streams to listen to.
    /// The most common (and default) stream is "trade_updates".
    void listen(const std::vector<std::string>& streams = {"trade_updates"});

    /// Stop receiving updates by sending an empty stream list.
    void unlisten();

    // ── callbacks ───────────────────────────────────────────────────────

    void on_trade_update(TradeUpdateHandler handler) { trade_handler_ = std::move(handler); }
    void on_auth(AuthHandler handler)                { auth_handler_  = std::move(handler); }

protected:
    void on_connected() override;
    void on_raw_message(const std::string& payload) override;
    void on_disconnected(const std::string& reason) override;

private:
    void handle_authorization(const nlohmann::json& data);
    void handle_listening(const nlohmann::json& data);
    void handle_trade_update(const nlohmann::json& data);

    std::string key_id_;
    std::string secret_key_;

    TradeUpdateHandler trade_handler_;
    AuthHandler        auth_handler_;

    std::atomic<bool> authenticated_{false};
    std::vector<std::string> pending_streams_{"trade_updates"};

    static constexpr const char* paper_host_ = "paper-api.alpaca.markets";
    static constexpr const char* live_host_  = "api.alpaca.markets";
    static constexpr const char* target_     = "/stream";
};

} // namespace alpaca
