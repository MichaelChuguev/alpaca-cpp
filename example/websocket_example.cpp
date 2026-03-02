#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

#include "alpaca/alpacaAPI.h"
#include "alpaca/api/websocket/AlpacaUpdatesStream.h"
#include "alpaca/api/websocket/AlpacaMarketDataStream.h"
#include "alpaca/core/error.h"

static std::atomic<bool> running{true};

void signal_handler(int) { running = false; }

int main() {
    std::signal(SIGINT, signal_handler);

    const char* key    = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");

    if (!key || !secret) {
        std::cerr << "Please set ALPACA_API_KEY and ALPACA_SECRET_KEY environment variables" << std::endl;
        return 1;
    }

    try {
        // ────────────────────────────────────────────────────────────────
        // 1. Trade / order updates  (binary frames)
        //    wss://paper-api.alpaca.markets/stream
        // ────────────────────────────────────────────────────────────────
        alpaca::AlpacaUpdatesStream updates(key, secret, alpaca::TraderAPIEndpoint::PAPER);
        
        updates.on_auth([](bool ok) {
            std::cout << "[updates] auth: " << (ok ? "authorized" : "FAILED") << "\n";
        });

        updates.on_trade_update([](const alpaca::TradeUpdate& tu) {
            std::cout << "[trade_update] event=" << alpaca::trade_update_event_to_string(tu.event)
                      << "  symbol=" << (tu.order ? tu.order->symbol : "n/a")
                      << "  qty=" << tu.qty
                      << "  price=" << tu.price
                      << "\n";
        });

        updates.on_error([](const std::string& err) {
            std::cerr << "[updates] error: " << err << "\n";
        });

        updates.start();   // connects → authenticates → listens for trade_updates

        // ────────────────────────────────────────────────────────────────
        // 2. Real-time market data  (text frames)
        //    wss://stream.data.alpaca.markets/v2/iex
        // ────────────────────────────────────────────────────────────────
        alpaca::AlpacaMarketDataStream data_stream(key, secret, alpaca::MarketDataFeed::IEX_FEED);

        data_stream.on_auth([&](bool ok) {
            std::cout << "[market-data] auth: " << (ok ? "authenticated" : "FAILED") << "\n";
        });

        data_stream.on_trade([](const alpaca::StreamTrade& t) {
            std::cout << "[trade] " << t.symbol
                      << "  price=" << t.price
                      << "  size=" << t.size
                      << "  ts=" << t.timestamp.to_string()
                      << "\n";
        });

        data_stream.on_quote([](const alpaca::StreamQuote& q) {
            std::cout << "[quote] " << q.symbol
                      << "  bid=" << q.bid_price << "x" << q.bid_size
                      << "  ask=" << q.ask_price << "x" << q.ask_size
                      << "\n";
        });

        data_stream.on_bar([](const alpaca::StreamBar& b) {
            std::cout << "[bar] " << b.symbol
                      << "  O=" << b.open << " H=" << b.high
                      << " L=" << b.low  << " C=" << b.close
                      << "  vol=" << b.volume
                      << "\n";
        });

        data_stream.on_stream_error([](int code, const std::string& msg) {
            std::cerr << "[market-data] error " << code << ": " << msg << "\n";
        });

        // Subscribe to quotes & bars for AAPL before starting
        data_stream.subscribe({"AAPL"}, {"AAPL"}, {"AAPL"});

        data_stream.start();   // connects → authenticates → subscribes

        // ────────────────────────────────────────────────────────────────
        // 3. FAKEPACA test stream  (text frames, always available)
        //    wss://stream.data.alpaca.markets/v2/test
        //
        //    The test stream produces synthetic data for the "FAKEPACA"
        //    symbol around the clock, making it ideal for verifying the
        //    WebSocket pipeline without needing live market hours.
        // ────────────────────────────────────────────────────────────────
        alpaca::AlpacaMarketDataStream test_stream(key, secret, alpaca::MarketDataFeed::TEST);

        test_stream.on_auth([](bool ok) {
            std::cout << "[test-stream] auth: " << (ok ? "authenticated" : "FAILED") << "\n";
        });

        test_stream.on_trade([](const alpaca::StreamTrade& t) {
            std::cout << "[test-trade] " << t.symbol
                      << "  price=" << t.price
                      << "  size=" << t.size
                      << "  ts=" << t.timestamp.to_string()
                      << "\n";
        });

        test_stream.on_quote([](const alpaca::StreamQuote& q) {
            std::cout << "[test-quote] " << q.symbol
                      << "  bid=" << q.bid_price << "x" << q.bid_size
                      << "  ask=" << q.ask_price << "x" << q.ask_size
                      << "\n";
        });

        test_stream.on_bar([](const alpaca::StreamBar& b) {
            std::cout << "[test-bar] " << b.symbol
                      << "  O=" << b.open << " H=" << b.high
                      << " L=" << b.low  << " C=" << b.close
                      << "  vol=" << b.volume
                      << "\n";
        });

        test_stream.on_stream_error([](int code, const std::string& msg) {
            std::cerr << "[test-stream] error " << code << ": " << msg << "\n";
        });

        // Subscribe to trades, quotes & bars for FAKEPACA before starting
        test_stream.subscribe({"FAKEPACA"}, {"FAKEPACA"}, {"FAKEPACA"});

        test_stream.start();   // connects → authenticates → subscribes

        // ────────────────────────────────────────────────────────────────
        // 4. Keep running until Ctrl-C
        // ────────────────────────────────────────────────────────────────
        std::cout << "\nStreaming… press Ctrl-C to stop.\n";
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        std::cout << "\nShutting down…\n";
        updates.stop();
        data_stream.stop();
        test_stream.stop();

    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
