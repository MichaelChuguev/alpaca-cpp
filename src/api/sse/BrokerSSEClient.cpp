#include "alpaca/BrokerSSEClient.h"

#include <sstream>
#include <utility>

#include "ixwebsocket/IXHttp.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Construction / lifecycle
// ---------------------------------------------------------------------------

BrokerSSEClient::BrokerSSEClient(const std::string& stream_url,
                                 const std::string& broker_api_key,
                                 const std::string& broker_api_secret)
    : stream_url_(stream_url)
    , broker_api_key_(broker_api_key)
    , broker_api_secret_(broker_api_secret)
    , use_token_provider_(false)
    , http_client_(false)
{
}

BrokerSSEClient::BrokerSSEClient(const std::string& stream_url,
                                 TokenProvider token_provider)
    : stream_url_(stream_url)
    , token_provider_(std::move(token_provider))
    , use_token_provider_(true)
    , http_client_(false)
{
}

BrokerSSEClient::~BrokerSSEClient() {
    stop();
}

// ---------------------------------------------------------------------------
// High-level API
// ---------------------------------------------------------------------------

void BrokerSSEClient::start() {
    if (running_.exchange(true)) {
        return;
    }

    stop_requested_.store(false);
    has_received_stream_data_.store(false);
    clear_pending_event();
    line_buffer_.clear();

    worker_thread_ = std::thread(&BrokerSSEClient::run, this);
}

void BrokerSSEClient::stop() {
    stop_requested_.store(true);

    {
        std::lock_guard<std::mutex> lock(request_mutex_);
        if (active_request_) {
            active_request_->cancel = true;
        }
    }

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }

    running_.store(false);
    set_state(State::STOPPED);
}

void BrokerSSEClient::set_auto_reconnect(bool enabled, std::chrono::milliseconds delay) {
    auto_reconnect_.store(enabled);
    reconnect_delay_ms_.store(static_cast<int>(delay.count()));
}

void BrokerSSEClient::on_event(EventHandler handler) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    event_handler_ = std::move(handler);
}

void BrokerSSEClient::on_json(JsonHandler handler) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    json_handler_ = std::move(handler);
}

void BrokerSSEClient::on_error(ErrorHandler handler) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    error_handler_ = std::move(handler);
}

void BrokerSSEClient::on_state_change(StateHandler handler) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    state_handler_ = std::move(handler);
}

// ---------------------------------------------------------------------------
// Stream loop
// ---------------------------------------------------------------------------

void BrokerSSEClient::run() {
    while (!stop_requested_.load()) {
        set_state(State::CONNECTING);
        has_received_stream_data_.store(false);

        auto request_args = std::make_shared<ix::HttpRequestArgs>();
        request_args->connectTimeout = 30;
        // Use long timeout to keep SSE effectively long-lived while
        // avoiding IXWebSocket assertions that reject zero-second timeouts.
        request_args->transferTimeout = 24 * 60 * 60;
        request_args->compress = false;
        request_args->extraHeaders["User-Agent"]     = "alpaca-cpp/1.0";
        request_args->extraHeaders["Accept"]         = "text/event-stream";
        request_args->extraHeaders["Cache-Control"]  = "no-cache";
        request_args->extraHeaders["Connection"]     = "keep-alive";

        if (use_token_provider_) {
            // Client-credentials flow: obtain a fresh Bearer token before
            // each connection attempt so reconnects always use a valid token.
            const std::string token = token_provider_();
            request_args->extraHeaders["Authorization"] = "Bearer " + token;
        } else {
            request_args->extraHeaders["APCA-API-KEY-ID"]     = broker_api_key_;
            request_args->extraHeaders["APCA-API-SECRET-KEY"] = broker_api_secret_;
        }

        request_args->onChunkCallback = [this](const std::string& chunk) {
            if (!has_received_stream_data_.exchange(true)) {
                set_state(State::STREAMING);
            }
            parse_chunk(chunk);
        };

        {
            std::lock_guard<std::mutex> lock(request_mutex_);
            active_request_ = request_args;
        }

        auto response = http_client_.get(stream_url_, request_args);

        {
            std::lock_guard<std::mutex> lock(request_mutex_);
            active_request_.reset();
        }

        dispatch_event();

        if (stop_requested_.load()) {
            break;
        }

        if (!response) {
            set_state(State::ERROR);
            emit_error("Broker SSE request failed with no response.");
        } else if (response->errorCode == ix::HttpErrorCode::Cancelled) {
            if (stop_requested_.load()) {
                break;
            }
            set_state(State::ERROR);
            emit_error("Broker SSE stream cancelled unexpectedly.");
        } else if (response->errorCode != ix::HttpErrorCode::Ok) {
            std::ostringstream oss;
            oss << "Broker SSE request error (" << static_cast<int>(response->errorCode)
                << "): " << response->errorMsg;
            set_state(State::ERROR);
            emit_error(oss.str());
        } else if (response->statusCode < 200 || response->statusCode >= 300) {
            std::ostringstream oss;
            oss << "Broker SSE HTTP error: " << response->statusCode;
            if (!response->description.empty()) {
                oss << " (" << response->description << ")";
            }
            set_state(State::ERROR);
            emit_error(oss.str());
        } else {
            set_state(State::ERROR);
            emit_error("Broker SSE stream closed by server.");
        }

        if (!auto_reconnect_.load() || stop_requested_.load()) {
            break;
        }

        sleep_reconnect_delay();
    }

    running_.store(false);
}

void BrokerSSEClient::parse_chunk(const std::string& chunk) {
    line_buffer_ += chunk;

    for (;;) {
        const std::size_t newline_pos = line_buffer_.find('\n');
        if (newline_pos == std::string::npos) {
            break;
        }

        std::string line = line_buffer_.substr(0, newline_pos);
        line_buffer_.erase(0, newline_pos + 1);

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        parse_line(line);
    }
}

// ---------------------------------------------------------------------------
// SSE parser
// ---------------------------------------------------------------------------

void BrokerSSEClient::parse_line(const std::string& line) {
    if (line.empty()) {
        dispatch_event();
        return;
    }

    if (line[0] == ':') {
        return;
    }

    const auto colon_pos = line.find(':');
    std::string field = line;
    std::string value;

    if (colon_pos != std::string::npos) {
        field = line.substr(0, colon_pos);
        value = line.substr(colon_pos + 1);
        if (!value.empty() && value[0] == ' ') {
            value.erase(0, 1);
        }
    }

    if (field == "event") {
        pending_event_type_ = value;
        return;
    }

    if (field == "data") {
        if (!pending_data_.empty()) {
            pending_data_ += '\n';
        }
        pending_data_ += value;
        return;
    }

    if (field == "id") {
        pending_event_id_ = value;
        return;
    }

    if (field == "retry") {
        try {
            pending_retry_ms_ = std::stoi(value);
        } catch (const std::exception&) {
            // Ignore invalid retry values.
        }
        return;
    }
}

void BrokerSSEClient::dispatch_event() {
    if (pending_event_type_.empty() && pending_data_.empty() && pending_event_id_.empty() &&
        pending_retry_ms_ < 0) {
        return;
    }

    Event event;
    event.event = pending_event_type_.empty() ? "message" : pending_event_type_;
    event.data = pending_data_;
    event.id = pending_event_id_;
    event.retry_ms = pending_retry_ms_;

    EventHandler event_handler;
    JsonHandler json_handler;
    {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        event_handler = event_handler_;
        json_handler = json_handler_;
    }

    if (event_handler) {
        event_handler(event);
    }

    if (json_handler && !event.data.empty()) {
        try {
            json_handler(nlohmann::json::parse(event.data));
        } catch (const nlohmann::json::exception&) {
            // Not all SSE events are guaranteed to contain JSON payloads.
        }
    }

    if (event.retry_ms >= 0) {
        reconnect_delay_ms_.store(event.retry_ms);
    }

    clear_pending_event();
}

void BrokerSSEClient::clear_pending_event() {
    pending_event_type_.clear();
    pending_event_id_.clear();
    pending_data_.clear();
    pending_retry_ms_ = -1;
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void BrokerSSEClient::set_state(State next_state) {
    const auto prev = state_.exchange(next_state);
    if (prev == next_state) {
        return;
    }

    StateHandler handler;
    {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        handler = state_handler_;
    }

    if (handler) {
        handler(next_state);
    }
}

void BrokerSSEClient::emit_error(const std::string& message) {
    ErrorHandler handler;
    {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        handler = error_handler_;
    }

    if (handler) {
        handler(message);
    }
}

void BrokerSSEClient::sleep_reconnect_delay() const {
    const int delay = reconnect_delay_ms_.load();
    if (delay <= 0) {
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

} // namespace alpaca
