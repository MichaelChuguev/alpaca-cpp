#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "alpaca/AlpacaBrokerAPI.h"

static std::atomic<bool> running{true};

void signal_handler(int) {
    running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);

    const char* key    = std::getenv("ALPACA_BROKER_API_KEY");
    const char* secret = std::getenv("ALPACA_BROKER_API_SECRET");
    const char* cc_id  = std::getenv("ALPACA_BROKER_CLIENT_ID");
    const char* cc_sec = std::getenv("ALPACA_BROKER_CLIENT_SECRET");

    // Client-credentials takes precedence if both CC vars are set.
    const bool use_cc = cc_id && cc_sec;

    if (!use_cc && (!key || !secret)) {
        std::cerr << "Set either:\n"
                  << "  ALPACA_BROKER_API_KEY + ALPACA_BROKER_API_SECRET  (legacy)\n"
                  << "  ALPACA_BROKER_CLIENT_ID + ALPACA_BROKER_CLIENT_SECRET  (client credentials)\n";
        return 1;
    }

    try {
        // ────────────────────────────────────────────────────────────────
        // 1. Build broker API and select an SSE stream endpoint
        // ────────────────────────────────────────────────────────────────
        std::unique_ptr<alpaca::AlpacaBrokerAPI> broker_ptr;
        if (use_cc) {
            std::cout << "[auth] Using client-credentials flow (automatic token refresh)\n";
            broker_ptr = std::make_unique<alpaca::AlpacaBrokerAPI>(
                cc_id, cc_sec,
                alpaca::BrokerAPIEndpoint::SANDBOX,
                alpaca::BrokerAuthMode::CLIENT_CREDENTIALS);
        } else {
            std::cout << "[auth] Using legacy key/secret flow\n";
            broker_ptr = std::make_unique<alpaca::AlpacaBrokerAPI>(
                key, secret, alpaca::BrokerAPIEndpoint::SANDBOX);
        }
        alpaca::AlpacaBrokerAPI& broker = *broker_ptr;

        auto sse = broker.create_system_event_v2_sse_client();

        // ────────────────────────────────────────────────────────────────
        // 2. Register callbacks
        // ────────────────────────────────────────────────────────────────
        sse.on_state_change([](alpaca::BrokerSSEClient::State state) {
            const char* name;
            switch (state) {
                case alpaca::BrokerSSEClient::State::STOPPED:    name = "STOPPED";    break;
                case alpaca::BrokerSSEClient::State::CONNECTING: name = "CONNECTING"; break;
                case alpaca::BrokerSSEClient::State::STREAMING:  name = "STREAMING";  break;
                case alpaca::BrokerSSEClient::State::ERROR:      name = "ERROR";      break;
                default:                                         name = "UNKNOWN";    break;
            }
            std::cout << "[sse] state=" << name << "\n";
        });

        sse.on_error([](const std::string& error) {
            std::cerr << "[sse] error: " << error << "\n";
        });

        sse.on_event([](const alpaca::BrokerSSEClient::Event& event) {
            std::cout << "[sse] event=" << event.event;
            if (!event.id.empty()) {
                std::cout << " id=" << event.id;
            }
            std::cout << " data=" << event.data << "\n";
        });

        // ────────────────────────────────────────────────────────────────
        // 3. Start streaming and wait until Ctrl-C
        // ────────────────────────────────────────────────────────────────
        std::cout << "Broker SSE stream running. Press Ctrl-C to stop.\n";
        sse.start();

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        std::cout << "\nShutting down...\n";
        sse.stop();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }
}
