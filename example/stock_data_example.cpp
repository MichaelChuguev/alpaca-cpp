#include <iostream>
#include <vector>
#include "alpaca/AlpacaAPI.h"
#include "alpaca/api/rest/marketdata/AlpacaMarketDataAPI.h"
#include "alpaca/core/types.h"

int main() {
    const char* key = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");
    
    if (!key || !secret) {
        std::cerr << "Please set ALPACA_API_KEY and ALPACA_SECRET_KEY environment variables" << std::endl;
        return 1;
    }

    try {
        alpaca::AlpacaMarketDataAPI api(key, secret, alpaca::MarketDataEndpoint::LIVE);

        // ─── Stock Data ──────────────────────────────────────────────────

        std::cout << "=== Stock Market Data Example ===" << std::endl;

        // 1. Get latest trade for a single stock
        std::cout << "\n1. Latest trade for AAPL..." << std::endl;
        auto trade = api.get_stock_latest_trade_single("AAPL", alpaca::DataFeed::IEX);
        std::cout << "  Price: " << trade.price 
                  << "  Size: " << trade.size 
                  << "  Exchange: " << trade.exchange << std::endl;

        // 2. Get latest quotes for multiple stocks
        std::cout << "\n2. Latest quotes for AAPL, MSFT, GOOGL..." << std::endl;
        auto quotes = api.get_stock_latest_quotes(
            {"AAPL", "MSFT", "GOOGL"}, alpaca::DataFeed::IEX);
        for (const auto& [sym, q] : quotes) {
            std::cout << "  " << sym 
                      << "  Bid: " << q.bid_price << " x " << q.bid_size
                      << "  Ask: " << q.ask_price << " x " << q.ask_size << std::endl;
        }

        // 3. Get historical bars for a single stock
        std::cout << "\n3. Daily bars for AAPL (last 5)..." << std::endl;
        auto bars = api.get_stock_bars_single(
            "AAPL", alpaca::DataTimeframe::ONE_DAY,
            "2026-03-01", "2026-03-08",
            alpaca::DataFeed::IEX, "", 5);
        for (const auto& bar : bars) {
            std::cout << "  " << bar.timestamp.to_string()
                      << "  O:" << bar.open << " H:" << bar.high 
                      << " L:" << bar.low << " C:" << bar.close 
                      << " V:" << bar.volume << std::endl;
        }

        // 4. Get snapshot for a single stock
        std::cout << "\n4. Snapshot for TSLA..." << std::endl;
        auto snapshot = api.get_stock_snapshot_single("TSLA", alpaca::DataFeed::IEX);
        std::cout << "  Latest trade price: " << snapshot.latest_trade.price << std::endl;
        std::cout << "  Daily bar close: " << snapshot.daily_bar.close << std::endl;

        // 5. Get latest bars for multiple stocks
        std::cout << "\n5. Latest bars for AAPL, MSFT..." << std::endl;
        auto latest_bars = api.get_stock_latest_bars(
            {"AAPL", "MSFT"}, alpaca::DataFeed::IEX);
        for (const auto& [sym, bar] : latest_bars) {
            std::cout << "  " << sym << "  Close: " << bar.close 
                      << "  Volume: " << bar.volume << std::endl;
        }

        // 6. Get stock meta exchanges
        std::cout << "\n6. Stock exchanges..." << std::endl;
        auto exchanges = api.get_stock_meta_exchanges();
        for (const auto& [code, name] : exchanges) {
            std::cout << "  " << code << " -> " << name << std::endl;
        }

        std::cout << "\n=== Stock Market Data Example Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
