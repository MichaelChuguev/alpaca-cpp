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

        std::cout << "=== News, Screener & Other Data Example ===" << std::endl;

        // 1. Get news articles
        std::cout << "\n1. Latest news articles..." << std::endl;
        auto news = api.get_news(
            {"AAPL", "TSLA"},         // symbols
            "2026-01-01T00:00:00Z",   // start
            "2026-03-15T00:00:00Z",   // end
            5,                         // limit
            "",                        // page_token
            alpaca::Sort::DESC,        // sort
            true);                     // include_content
        for (const auto& article : news) {
            std::cout << "  [" << article.created_at.to_string() << "] " 
                      << article.headline << std::endl;
            std::cout << "    Source: " << article.source 
                      << "  Author: " << article.author << std::endl;
            if (!article.symbols.empty()) {
                std::cout << "    Symbols:";
                for (const auto& sym : article.symbols) {
                    std::cout << " " << sym;
                }
                std::cout << std::endl;
            }
        }

        // 2. Get most active stocks
        std::cout << "\n2. Most active stocks..." << std::endl;
        auto actives = api.get_most_actives(5);
        for (const auto& stock : actives) {
            std::cout << "  " << stock.symbol 
                      << "  Volume: " << stock.volume
                      << "  Trade count: " << stock.trade_count << std::endl;
        }

        // 3. Get stock movers
        std::cout << "\n3. Top stock movers..." << std::endl;
        auto movers = api.get_movers(
            alpaca::ScreenerMarketType::STOCKS, 5);
        for (const auto& m : movers) {
            std::cout << "  " << m.symbol
                      << "  Price: " << m.price
                      << "  Change: " << m.change
                      << "  Percent change: " << m.percent_change << "%" << std::endl;
        }

        // 4. Get forex latest rates
        std::cout << "\n4. Latest forex rates..." << std::endl;
        try {
            auto rates = api.get_forex_latest_rates({"USD/EUR", "USD/GBP", "USD/JPY"});
            for (const auto& [pair, rate] : rates) {
                std::cout << "  " << pair
                          << "  Bid: " << rate.bid_price
                          << "  Ask: " << rate.ask_price
                          << "  Mid: " << rate.mid_price << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  Skipped (requires premium subscription): " << e.what() << std::endl;
        }

        // 5. Get historical forex rates
        std::cout << "\n5. Historical forex rates for USD/EUR..." << std::endl;
        try {
            auto hist_rates = api.get_forex_rates(
                {"USD/EUR"},
                alpaca::DataTimeframe::ONE_DAY,
                "2026-03-01", "2026-03-08",
                5);
            for (const auto& [pair, rate_vec] : hist_rates) {
                std::cout << "  " << pair << ": " << rate_vec.size() << " entries" << std::endl;
                for (const auto& r : rate_vec) {
                    std::cout << "    " << r.timestamp.to_string()
                              << "  Bid: " << r.bid_price
                              << "  Ask: " << r.ask_price << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "  Skipped (requires premium subscription): " << e.what() << std::endl;
        }

        // 6. Get fixed income latest prices
        std::cout << "\n6. Latest fixed income prices..." << std::endl;
        try {
            auto fi_prices = api.get_fixed_income_latest_prices({"US10Y", "US30Y", "US2Y"});
            for (const auto& [sym, price] : fi_prices) {
                std::cout << "  " << sym
                          << "  Rate: " << price.rate << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  Skipped (requires premium subscription): " << e.what() << std::endl;
        }

        // 7. Get logo URL
        std::cout << "\n7. Logo URL for AAPL..." << std::endl;
        try {
            auto logo_url = api.get_logo_url("AAPL");
            std::cout << "  URL: " << logo_url << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << std::endl;
        }

        // 8. Get corporate actions (raw JSON)
        std::cout << "\n8. Corporate actions..." << std::endl;
        try {
            auto actions = api.get_corporate_actions(
                {"AAPL", "MSFT"},
                {"cash_dividend"},
                "2026-01-01", "2026-03-15",
                5, "",
                alpaca::Sort::DESC);
            std::cout << "  Raw JSON response contains " << actions.size()
                      << " top-level keys" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << std::endl;
        }

        std::cout << "\n=== News, Screener & Other Data Example Complete ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
