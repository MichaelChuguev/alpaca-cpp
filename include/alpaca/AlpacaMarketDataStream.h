#pragma once

#include <string>
#include <functional>
#include <vector>
#include <atomic>

#include "alpaca/api/websocket/WebSocketClient.h"
#include "alpaca/model/stream/market_data.h"
#include "alpaca/core/types.h"

namespace alpaca {

/// Feed selection for the market-data stream.
enum class MarketDataFeed {
    IEX_FEED,   ///< Free tier — IEX only
    SIP_FEED,   ///< Full SIP feed (paid plans)
    TEST        ///< Test stream (always available)
};

/// WebSocket stream for real-time market data (trades, quotes, bars).
///
/// Connects to:
///   Live:    wss://stream.data.alpaca.markets/v2/{feed}
///   Sandbox: wss://stream.data.sandbox.alpaca.markets/v2/{feed}
/// where {feed} is one of: iex, sip, test
///
/// **Uses text WebSocket frames** (JSON arrays).
///
/// Protocol:
///   1. Receive welcome: [{"T":"success","msg":"connected"}]
///   2. Send auth:       {"action":"auth","key":"...","secret":"..."}
///   3. Receive auth OK: [{"T":"success","msg":"authenticated"}]
///   4. Subscribe:       {"action":"subscribe","trades":["AAPL"],"quotes":["AAPL"]}
///   5. Receive sub ack: [{"T":"subscription","trades":["AAPL"],"quotes":["AAPL"],...}]
///   6. Receive data:    [{"T":"t","S":"AAPL","p":150.25,...}]
class AlpacaMarketDataStream : public WebSocketClient {
public:
    using TradeHandler  = std::function<void(const StreamTrade&)>;
    using QuoteHandler  = std::function<void(const StreamQuote&)>;
    using BarHandler    = std::function<void(const StreamBar&)>;
    using AuthHandler   = std::function<void(bool)>;
    using ErrorHandler  = std::function<void(int /*code*/, const std::string& /*msg*/)>;

    /// Construct with API credentials and feed selection.
    AlpacaMarketDataStream(const std::string& key_id,
                           const std::string& secret_key,
                           MarketDataFeed feed = MarketDataFeed::IEX_FEED,
                           MarketDataEndpoint endpoint = MarketDataEndpoint::LIVE);

    // ── high-level API ──────────────────────────────────────────────────

    /// Connect, authenticate, and begin receiving data.
    void start();

    /// Disconnect.
    void stop();

    /// Subscribe to one or more symbols for trades, quotes, and/or bars.
    /// Only the channels you pass will be added; existing subscriptions
    /// are preserved by the server.
    void subscribe(const std::vector<std::string>& trades  = {},
                   const std::vector<std::string>& quotes  = {},
                   const std::vector<std::string>& bars    = {});

    /// Unsubscribe from one or more symbols / channels.
    void unsubscribe(const std::vector<std::string>& trades = {},
                     const std::vector<std::string>& quotes = {},
                     const std::vector<std::string>& bars   = {});

    // ── callbacks ───────────────────────────────────────────────────────

    void on_trade(TradeHandler handler)              { trade_handler_ = std::move(handler); }
    void on_quote(QuoteHandler handler)              { quote_handler_ = std::move(handler); }
    void on_bar(BarHandler handler)                  { bar_handler_   = std::move(handler); }
    void on_auth(AuthHandler handler)                { auth_handler_  = std::move(handler); }
    void on_stream_error(ErrorHandler handler)       { stream_error_handler_ = std::move(handler); }

protected:
    void on_connected() override;
    void on_raw_message(const std::string& payload) override;
    void on_disconnected(const std::string& reason) override;

private:
    void dispatch_message(const nlohmann::json& msg);
    static std::string feed_to_string(MarketDataFeed f);
    static std::string data_host(MarketDataEndpoint endpoint);

    std::string    key_id_;
    std::string    secret_key_;
    MarketDataFeed feed_;
    MarketDataEndpoint endpoint_;

    TradeHandler  trade_handler_;
    QuoteHandler  quote_handler_;
    BarHandler    bar_handler_;
    AuthHandler   auth_handler_;
    ErrorHandler  stream_error_handler_;

    std::atomic<bool> authenticated_{false};

    // Pending subscriptions cached before auth completes
    std::vector<std::string> pending_trades_;
    std::vector<std::string> pending_quotes_;
    std::vector<std::string> pending_bars_;

    static constexpr const char* live_data_host_    = "stream.data.alpaca.markets";
    static constexpr const char* sandbox_data_host_ = "stream.data.sandbox.alpaca.markets";
};

/// WebSocket stream for real-time option market data (trades, quotes).
///
/// Connects to:
///   Live:    wss://stream.data.alpaca.markets/v1beta1/{feed}
///   Sandbox: wss://stream.data.sandbox.alpaca.markets/v1beta1/{feed}
/// where {feed} is one of: indicative, opra
///
/// **Uses binary WebSocket frames** with `Content-Type: application/msgpack`.
///
/// Protocol:
///   1. Receive welcome: [{"T":"success","msg":"connected"}]
///   2. Send auth:       {"action":"auth","key":"...","secret":"..."}
///   3. Receive auth OK: [{"T":"success","msg":"authenticated"}]
///   4. Subscribe:       {"action":"subscribe","trades":["AAPL240315C00172500"]}
///   5. Receive sub ack: [{"T":"subscription","trades":["..."],"quotes":["..."]}]
///   6. Receive data:    [{"T":"t","S":"AAPL240315C00172500","p":2.84,...}]
class AlpacaOptionMarketDataStream : public WebSocketClient {
public:
    using TradeHandler = std::function<void(const StreamOptionTrade&)>;
    using QuoteHandler = std::function<void(const StreamOptionQuote&)>;
    using AuthHandler  = std::function<void(bool)>;
    using ErrorHandler = std::function<void(int /*code*/, const std::string& /*msg*/)>;

    /// Construct with API credentials, option feed, and endpoint selection.
    AlpacaOptionMarketDataStream(const std::string& key_id,
                                 const std::string& secret_key,
                                 OptionFeed feed = OptionFeed::INDICATIVE,
                                 MarketDataEndpoint endpoint = MarketDataEndpoint::LIVE);

    // ── high-level API ──────────────────────────────────────────────────

    /// Connect, authenticate, and begin receiving data.
    void start();

    /// Disconnect.
    void stop();

    /// Subscribe to one or more option symbols for trades and/or quotes.
    /// Only the channels you pass will be added; existing subscriptions
    /// are preserved by the server.
    void subscribe(const std::vector<std::string>& trades = {},
                   const std::vector<std::string>& quotes = {});

    /// Unsubscribe from one or more option symbols / channels.
    void unsubscribe(const std::vector<std::string>& trades = {},
                     const std::vector<std::string>& quotes = {});

    // ── callbacks ───────────────────────────────────────────────────────

    void on_trade(TradeHandler handler)        { trade_handler_ = std::move(handler); }
    void on_quote(QuoteHandler handler)        { quote_handler_ = std::move(handler); }
    void on_auth(AuthHandler handler)          { auth_handler_ = std::move(handler); }
    void on_stream_error(ErrorHandler handler) { stream_error_handler_ = std::move(handler); }

protected:
    void on_connected() override;
    void on_raw_message(const std::string& payload) override;
    void on_disconnected(const std::string& reason) override;

private:
    void dispatch_message(const nlohmann::json& msg);
    static std::string feed_to_string(OptionFeed feed);
    static std::string data_host(MarketDataEndpoint endpoint);

    std::string key_id_;
    std::string secret_key_;
    OptionFeed feed_;
    MarketDataEndpoint endpoint_;

    TradeHandler trade_handler_;
    QuoteHandler quote_handler_;
    AuthHandler auth_handler_;
    ErrorHandler stream_error_handler_;

    std::atomic<bool> authenticated_{false};

    // Pending subscriptions cached before auth completes
    std::vector<std::string> pending_trades_;
    std::vector<std::string> pending_quotes_;

    static constexpr const char* live_data_host_    = "stream.data.alpaca.markets";
    static constexpr const char* sandbox_data_host_ = "stream.data.sandbox.alpaca.markets";
};

} // namespace alpaca
