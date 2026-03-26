#include "alpaca/AlpacaMarketDataStream.h"

#include <iostream>

namespace alpaca {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

std::string AlpacaMarketDataStream::feed_to_string(MarketDataFeed f) {
    switch (f) {
        case MarketDataFeed::SIP_FEED: return "sip";
        case MarketDataFeed::TEST:     return "test";
        case MarketDataFeed::IEX_FEED:
        default:                       return "iex";
    }
}

std::string AlpacaMarketDataStream::data_host(MarketDataEndpoint endpoint) {
    return endpoint == MarketDataEndpoint::SANDBOX ? sandbox_data_host_ : live_data_host_;
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

AlpacaMarketDataStream::AlpacaMarketDataStream(const std::string& key_id,
                                               const std::string& secret_key,
                                               MarketDataFeed feed,
                                               MarketDataEndpoint endpoint)
    : WebSocketClient(data_host(endpoint),
                      "/v2/" + feed_to_string(feed),
                      /*use_binary=*/false),   // ← text frames for data stream
      key_id_(key_id),
      secret_key_(secret_key),
      feed_(feed),
      endpoint_(endpoint)
{}

// ---------------------------------------------------------------------------
// High-level API
// ---------------------------------------------------------------------------

void AlpacaMarketDataStream::start() {
    authenticated_ = false;
    connect();
}

void AlpacaMarketDataStream::stop() {
    disconnect();
}

void AlpacaMarketDataStream::subscribe(const std::vector<std::string>& trades,
                                       const std::vector<std::string>& quotes,
                                       const std::vector<std::string>& bars)
{
    // Cache for after authentication if not authed yet
    if (!authenticated_) {
        pending_trades_ = trades;
        pending_quotes_ = quotes;
        pending_bars_   = bars;
        return;
    }

    nlohmann::json msg = {{"action", "subscribe"}};
    if (!trades.empty()) msg["trades"] = trades;
    if (!quotes.empty()) msg["quotes"] = quotes;
    if (!bars.empty())   msg["bars"]   = bars;
    send(msg);
}

void AlpacaMarketDataStream::unsubscribe(const std::vector<std::string>& trades,
                                         const std::vector<std::string>& quotes,
                                         const std::vector<std::string>& bars)
{
    nlohmann::json msg = {{"action", "unsubscribe"}};
    if (!trades.empty()) msg["trades"] = trades;
    if (!quotes.empty()) msg["quotes"] = quotes;
    if (!bars.empty())   msg["bars"]   = bars;
    send(msg);
}

// ---------------------------------------------------------------------------
// WebSocketClient hooks
// ---------------------------------------------------------------------------

void AlpacaMarketDataStream::on_connected() {
    // The data stream sends a welcome message first:
    //   [{"T":"success","msg":"connected"}]
    // We wait for that welcome in on_raw_message before sending auth.
    // So nothing to do here — auth is triggered upon receiving the welcome.
}

void AlpacaMarketDataStream::on_raw_message(const std::string& payload) {
    try {
        auto arr = nlohmann::json::parse(payload);

        // The market-data stream always sends JSON arrays
        if (!arr.is_array()) {
            // Unusual — try wrapping
            arr = nlohmann::json::array({arr});
        }

        for (auto& msg : arr) {
            dispatch_message(msg);
        }
    } catch (const nlohmann::json::exception&) {
        // Could not parse
    }
}

void AlpacaMarketDataStream::on_disconnected(const std::string& reason) {
    authenticated_ = false;
}

// ---------------------------------------------------------------------------
// Per-message dispatch
// ---------------------------------------------------------------------------

void AlpacaMarketDataStream::dispatch_message(const nlohmann::json& msg) {
    std::string type = msg.value("T", "");

    // ── control messages ────────────────────────────────────────────────

    if (type == "success") {
        std::string m = msg.value("msg", "");
        if (m == "connected") {
            // Welcome received — now authenticate
            nlohmann::json auth = {
                {"action", "auth"},
                {"key",    key_id_},
                {"secret", secret_key_}
            };
            send(auth);
        } else if (m == "authenticated") {
            authenticated_ = true;
            set_state(State::AUTHENTICATED);
            if (auth_handler_) auth_handler_(true);

            // Flush any pending subscriptions
            if (!pending_trades_.empty() || !pending_quotes_.empty() || !pending_bars_.empty()) {
                subscribe(pending_trades_, pending_quotes_, pending_bars_);
                pending_trades_.clear();
                pending_quotes_.clear();
                pending_bars_.clear();
            }
        }
        return;
    }

    if (type == "error") {
        int code = msg.value("code", 0);
        std::string m = msg.value("msg", "unknown error");

        if (code == 402) {
            // Auth failed
            if (auth_handler_) auth_handler_(false);
        }

        if (stream_error_handler_) {
            stream_error_handler_(code, m);
        }
        return;
    }

    if (type == "subscription") {
        // Subscription acknowledgement — no-op by default.
        return;
    }

    // ── data messages ───────────────────────────────────────────────────

    if (type == "t" && trade_handler_) {
        trade_handler_(StreamTrade::from_json(msg));
        return;
    }

    if (type == "q" && quote_handler_) {
        quote_handler_(StreamQuote::from_json(msg));
        return;
    }

    if (type == "b" && bar_handler_) {
        bar_handler_(StreamBar::from_json(msg));
        return;
    }

    // Other message types (daily bars, statuses, lulds, corrections, etc.)
    // can be added here following the same pattern.
}

// ---------------------------------------------------------------------------
// Option stream helpers
// ---------------------------------------------------------------------------

std::string AlpacaOptionMarketDataStream::feed_to_string(OptionFeed feed) {
    switch (feed) {
        case OptionFeed::OPRA:       return "opra";
        case OptionFeed::INDICATIVE:
        case OptionFeed::DEFAULT:
        default:                     return "indicative";
    }
}

std::string AlpacaOptionMarketDataStream::data_host(MarketDataEndpoint endpoint) {
    return endpoint == MarketDataEndpoint::SANDBOX ? sandbox_data_host_ : live_data_host_;
}

// ---------------------------------------------------------------------------
// Option stream construction
// ---------------------------------------------------------------------------

AlpacaOptionMarketDataStream::AlpacaOptionMarketDataStream(const std::string& key_id,
                                                           const std::string& secret_key,
                                                           OptionFeed feed,
                                                           MarketDataEndpoint endpoint)
    : WebSocketClient(data_host(endpoint),
                      "/v1beta1/" + feed_to_string(feed),
                      /*use_binary=*/true),
      key_id_(key_id),
      secret_key_(secret_key),
      feed_(feed == OptionFeed::DEFAULT ? OptionFeed::INDICATIVE : feed),
      endpoint_(endpoint)
{
    set_extra_headers({{"Content-Type", "application/msgpack"}});
}

// ---------------------------------------------------------------------------
// Option stream high-level API
// ---------------------------------------------------------------------------

void AlpacaOptionMarketDataStream::start() {
    authenticated_ = false;
    connect();
}

void AlpacaOptionMarketDataStream::stop() {
    disconnect();
}

void AlpacaOptionMarketDataStream::subscribe(const std::vector<std::string>& trades,
                                             const std::vector<std::string>& quotes)
{
    // Cache for after authentication if not authed yet
    if (!authenticated_) {
        pending_trades_ = trades;
        pending_quotes_ = quotes;
        return;
    }

    nlohmann::json msg = {{"action", "subscribe"}};
    if (!trades.empty()) msg["trades"] = trades;
    if (!quotes.empty()) msg["quotes"] = quotes;
    send_msgpack(msg);
}

void AlpacaOptionMarketDataStream::unsubscribe(const std::vector<std::string>& trades,
                                               const std::vector<std::string>& quotes)
{
    nlohmann::json msg = {{"action", "unsubscribe"}};
    if (!trades.empty()) msg["trades"] = trades;
    if (!quotes.empty()) msg["quotes"] = quotes;
    send_msgpack(msg);
}

// ---------------------------------------------------------------------------
// Option stream WebSocketClient hooks
// ---------------------------------------------------------------------------

void AlpacaOptionMarketDataStream::on_connected() {
    // The option data stream sends a welcome message first:
    //   [{"T":"success","msg":"connected"}]
    // We wait for that welcome in on_raw_message before sending auth.
}

void AlpacaOptionMarketDataStream::on_raw_message(const std::string& payload) {
    try {
        auto arr = nlohmann::json::from_msgpack(payload.begin(), payload.end());

        // The option market-data stream always sends arrays
        if (!arr.is_array()) {
            arr = nlohmann::json::array({arr});
        }

        for (auto& msg : arr) {
            dispatch_message(msg);
        }
    } catch (const nlohmann::json::exception&) {
        // Could not parse — invalid/corrupt MessagePack frame
    }
}

void AlpacaOptionMarketDataStream::on_disconnected(const std::string& reason) {
    authenticated_ = false;
}

// ---------------------------------------------------------------------------
// Option stream per-message dispatch
// ---------------------------------------------------------------------------

void AlpacaOptionMarketDataStream::dispatch_message(const nlohmann::json& msg) {
    std::string type = msg.value("T", "");

    // ── control messages ────────────────────────────────────────────────

    if (type == "success") {
        std::string m = msg.value("msg", "");
        if (m == "connected") {
            // Welcome received — now authenticate
            nlohmann::json auth = {
                {"action", "auth"},
                {"key",    key_id_},
                {"secret", secret_key_}
            };
            send_msgpack(auth);
        } else if (m == "authenticated") {
            authenticated_ = true;
            set_state(State::AUTHENTICATED);
            if (auth_handler_) auth_handler_(true);

            // Flush any pending subscriptions
            if (!pending_trades_.empty() || !pending_quotes_.empty()) {
                subscribe(pending_trades_, pending_quotes_);
                pending_trades_.clear();
                pending_quotes_.clear();
            }
        }
        return;
    }

    if (type == "error") {
        int code = msg.value("code", 0);
        std::string m = msg.value("msg", "unknown error");

        if (code == 402) {
            // Auth failed
            if (auth_handler_) auth_handler_(false);
        }

        if (stream_error_handler_) {
            stream_error_handler_(code, m);
        }
        return;
    }

    if (type == "subscription") {
        // Subscription acknowledgement — no-op by default.
        return;
    }

    // ── data messages ───────────────────────────────────────────────────

    if (type == "t" && trade_handler_) {
        trade_handler_(StreamOptionTrade::from_json(msg));
        return;
    }

    if (type == "q" && quote_handler_) {
        quote_handler_(StreamOptionQuote::from_json(msg));
        return;
    }
}

} // namespace alpaca
