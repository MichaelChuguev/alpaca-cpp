#include "alpaca/api/websocket/AlpacaUpdatesStream.h"

#include <iostream>

namespace alpaca {

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

AlpacaUpdatesStream::AlpacaUpdatesStream(const std::string& key_id,
                                         const std::string& secret_key,
                                         TraderAPIEndpoint  endpoint)
    : WebSocketClient(endpoint == PAPER ? paper_host_ : live_host_,
                      target_,
                      /*use_binary=*/true),   // ← binary frames for this endpoint
      key_id_(key_id),
      secret_key_(secret_key)
{}

// ---------------------------------------------------------------------------
// High-level API
// ---------------------------------------------------------------------------

void AlpacaUpdatesStream::start() {
    authenticated_ = false;
    connect();
}

void AlpacaUpdatesStream::stop() {
    unlisten();
    disconnect();
}

void AlpacaUpdatesStream::listen(const std::vector<std::string>& streams) {
    pending_streams_ = streams;
    if (authenticated_) {
        nlohmann::json msg = {
            {"action", "listen"},
            {"data", {{"streams", streams}}}
        };
        send(msg);
    }
}

void AlpacaUpdatesStream::unlisten() {
    nlohmann::json msg = {
        {"action", "listen"},
        {"data", {{"streams", nlohmann::json::array()}}}
    };
    send(msg);
}

// ---------------------------------------------------------------------------
// WebSocketClient hooks
// ---------------------------------------------------------------------------

void AlpacaUpdatesStream::on_connected() {
    // Step 1: Send authentication
    nlohmann::json auth = {
        {"action", "auth"},
        {"key",    key_id_},
        {"secret", secret_key_}
    };
    send(auth);
}

void AlpacaUpdatesStream::on_raw_message(const std::string& payload) {
    try {
        auto j = nlohmann::json::parse(payload);

        // Dispatch based on "stream" field
        std::string stream = j.value("stream", "");

        if (stream == "authorization") {
            handle_authorization(j["data"]);
        } else if (stream == "listening") {
            handle_listening(j["data"]);
        } else if (stream == "trade_updates") {
            handle_trade_update(j["data"]);
        } else {
            // Check for error action
            std::string action = j.value("action", "");
            if (action == "error") {
                std::string err = j["data"].value("error_message", "unknown error");
                // Errors on this stream result in connection closure per the spec
            }
        }
    } catch (const nlohmann::json::exception& e) {
        // Could not parse — binary/corrupt frame
    }
}

void AlpacaUpdatesStream::on_disconnected(const std::string& reason) {
    authenticated_ = false;
}

// ---------------------------------------------------------------------------
// Internal handlers
// ---------------------------------------------------------------------------

void AlpacaUpdatesStream::handle_authorization(const nlohmann::json& data) {
    std::string status = data.value("status", "");
    if (status == "authorized") {
        authenticated_ = true;
        set_state(State::AUTHENTICATED);
        if (auth_handler_) auth_handler_(true);

        // Automatically subscribe to the pending streams
        listen(pending_streams_);
    } else {
        authenticated_ = false;
        set_state(State::ERROR);
        if (auth_handler_) auth_handler_(false);
    }
}

void AlpacaUpdatesStream::handle_listening(const nlohmann::json& data) {
    // Acknowledgement — nothing special to do.
    // The user can observe state through on_message if desired.
}

void AlpacaUpdatesStream::handle_trade_update(const nlohmann::json& data) {
    if (trade_handler_) {
        TradeUpdate tu = TradeUpdate::from_json(data);
        trade_handler_(tu);
    }
}

} // namespace alpaca
