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

        std::cout << "=== Crypto Market Data Example ===" << std::endl;

        // 1. Get crypto trades
        std::cout << "\n1. Recent BTC/USD trades..." << std::endl;
        auto trades = api.get_crypto_trades(
            {"BTC/USD"}, alpaca::CryptoLoc::US,
            "2026-03-01T00:00:00Z", "2026-03-01T01:00:00Z",
            5);
        for (const auto& [sym, trade_vec] : trades) {
            std::cout << "  " << sym << ": " << trade_vec.size() << " trades" << std::endl;
            for (const auto& t : trade_vec) {
                std::cout << "    Price: " << t.price 
                          << " Size: " << t.size
                          << " Taker: " << t.taker_side << std::endl;
            }
        }

        // 2. Get crypto bars
        std::cout << "\n2. Hourly BTC/USD bars..." << std::endl;
        auto bars = api.get_crypto_bars(
            {"BTC/USD"}, alpaca::CryptoLoc::US,
            alpaca::DataTimeframe::ONE_HOUR,
            "2026-03-01T00:00:00Z", "2026-03-01T06:00:00Z",
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

        // 3. Get latest crypto trades
        std::cout << "\n3. Latest crypto trades for BTC/USD, ETH/USD..." << std::endl;
        auto latest_trades = api.get_crypto_latest_trades(
            {"BTC/USD", "ETH/USD"}, alpaca::CryptoLoc::US);
        for (const auto& [sym, t] : latest_trades) {
            std::cout << "  " << sym << "  Price: " << t.price
                      << "  Size: " << t.size << std::endl;
        }

        // 4. Get latest crypto quotes
        std::cout << "\n4. Latest crypto quotes..." << std::endl;
        auto latest_quotes = api.get_crypto_latest_quotes(
            {"BTC/USD", "ETH/USD"}, alpaca::CryptoLoc::US);
        for (const auto& [sym, q] : latest_quotes) {
            std::cout << "  " << sym
                      << "  Bid: " << q.bid_price << " x " << q.bid_size
                      << "  Ask: " << q.ask_price << " x " << q.ask_size << std::endl;
        }

        // 5. Get latest crypto orderbooks
        std::cout << "\n5. Latest BTC/USD orderbook..." << std::endl;
        auto orderbooks = api.get_crypto_latest_orderbooks(
            {"BTC/USD"}, alpaca::CryptoLoc::US);
        for (const auto& [sym, ob] : orderbooks) {
            std::cout << "  " << sym << std::endl;
            std::cout << "  Top bids:" << std::endl;
            for (size_t i = 0; i < std::min(ob.bids.size(), size_t(3)); ++i) {
                std::cout << "    " << ob.bids[i].price << " x " << ob.bids[i].size << std::endl;
            }
            std::cout << "  Top asks:" << std::endl;
            for (size_t i = 0; i < std::min(ob.asks.size(), size_t(3)); ++i) {
                std::cout << "    " << ob.asks[i].price << " x " << ob.asks[i].size << std::endl;
            }
        }

        // 6. Get crypto snapshots
        std::cout << "\n6. Crypto snapshots..." << std::endl;
        auto snapshots = api.get_crypto_snapshots(
            {"BTC/USD", "ETH/USD"}, alpaca::CryptoLoc::US);
        for (const auto& [sym, snap] : snapshots) {
            std::cout << "  " << sym 
                      << "  Latest trade: " << snap.latest_trade.price
                      << "  Daily bar close: " << snap.daily_bar.close << std::endl;
        }

        // 7. Get latest crypto bars
        std::cout << "\n7. Latest crypto bars..." << std::endl;
        auto latest_bars = api.get_crypto_latest_bars(
            {"BTC/USD", "ETH/USD"}, alpaca::CryptoLoc::US);
        for (const auto& [sym, bar] : latest_bars) {
            std::cout << "  " << sym << "  Close: " << bar.close
                      << "  Volume: " << bar.volume << std::endl;
        }

        std::cout << "\n=== Crypto Market Data Example Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
