#include <iostream>
#include <vector>
#include "alpaca/AlpacaMarketDataAPI.h"
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

        std::cout << "=== Option Market Data Example ===" << std::endl;

        // 1. Get option bars
        std::cout << "\n1. Option bars for AAPL250321C00200000..." << std::endl;
        auto bars = api.get_option_bars(
            {"AAPL250321C00200000"},
            alpaca::DataTimeframe::ONE_DAY,
            "2025-03-01", "2025-03-15",
            5);
        for (const auto& [sym, bar_vec] : bars) {
            std::cout << "  " << sym << ": " << bar_vec.size() << " bars" << std::endl;
            for (const auto& bar : bar_vec) {
                std::cout << "    " << bar.timestamp.to_string()
                          << " O:" << bar.open << " H:" << bar.high
                          << " L:" << bar.low << " C:" << bar.close
                          << " V:" << bar.volume << std::endl;
            }
        }

        // 2. Get latest option trades
        std::cout << "\n2. Latest option trades..." << std::endl;
        auto trades = api.get_option_latest_trades(
            {"AAPL250321C00200000", "MSFT250321C00400000"},
            alpaca::OptionFeed::INDICATIVE);
        for (const auto& [sym, t] : trades) {
            std::cout << "  " << sym << "  Price: " << t.price
                      << "  Size: " << t.size << std::endl;
        }

        // 3. Get latest option quotes
        std::cout << "\n3. Latest option quotes..." << std::endl;
        auto quotes = api.get_option_latest_quotes(
            {"AAPL250321C00200000", "MSFT250321C00400000"},
            alpaca::OptionFeed::INDICATIVE);
        for (const auto& [sym, q] : quotes) {
            std::cout << "  " << sym
                      << "  Bid: " << q.bid_price << " x " << q.bid_size
                      << "  Ask: " << q.ask_price << " x " << q.ask_size << std::endl;
        }

        // 4. Get option snapshots
        std::cout << "\n4. Option snapshots..." << std::endl;
        auto snapshots = api.get_option_snapshots(
            {"AAPL250321C00200000"},
            alpaca::OptionFeed::INDICATIVE);
        for (const auto& [sym, snap] : snapshots) {
            std::cout << "  " << sym << std::endl;
            std::cout << "    Latest trade price: " << snap.latest_trade.price << std::endl;
            std::cout << "    Implied vol: " << snap.implied_volatility << std::endl;
        }

        // 5. Get option chain snapshot for an underlying
        std::cout << "\n5. Option chain snapshot for AAPL..." << std::endl;
        auto chain = api.get_option_chain("AAPL", alpaca::OptionFeed::INDICATIVE);
        std::cout << "  Found " << chain.size() << " contracts in chain" << std::endl;
        int count = 0;
        for (const auto& [sym, snap] : chain) {
            if (count++ >= 3) break;  // Just show first 3
            std::cout << "  " << sym 
                      << "  IV: " << snap.implied_volatility << std::endl;
        }

        // 6. Get option meta exchanges
        std::cout << "\n6. Option exchanges..." << std::endl;
        auto exchanges = api.get_option_meta_exchanges();
        for (const auto& [code, name] : exchanges) {
            std::cout << "  " << code << " -> " << name << std::endl;
        }

        std::cout << "\n=== Option Market Data Example Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
