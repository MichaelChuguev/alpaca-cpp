#include <iostream>
#include "alpaca/alpacaAPI.h"
#include "alpaca/api/rest/trader/AlpacaTraderAPI.h"
#include "alpaca/core/error.h"

int main() {
    // Get API credentials from environment variables
    const char* key = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");
    
    if (!key || !secret) {
        std::cerr << "Please set ALPACA_API_KEY and ALPACA_SECRET_KEY environment variables" << std::endl;
        return 1;
    }

    try {
        // Initialize the API (using paper trading)
        alpaca::AlpacaTraderAPI api(key, secret, alpaca::TraderAPIEndpoint::PAPER);

        // Get account information
        std::cout << "=== Account Information ===" << std::endl;
        auto account = api.get_account();
        std::cout << "Account ID: " << account.id << std::endl;
        std::cout << "Cash: $" << account.cash << std::endl;
        std::cout << "Buying Power: $" << account.buying_power << std::endl;
        std::cout << "Portfolio Value: $" << account.portfolio_value << std::endl;
        std::cout << "Status: " << account.status << std::endl;

        // Get market clock
        std::cout << "\n=== Market Clock ===" << std::endl;
        auto clock = api.get_clock();
        std::cout << "Market is " << (clock.is_open ? "OPEN" : "CLOSED") << std::endl;
        std::cout << "Current Time: " << clock.timestamp.to_string() << std::endl;
        std::cout << "Next Open: " << clock.next_open.to_string() << std::endl;
        std::cout << "Next Close: " << clock.next_close.to_string() << std::endl;

        // Get all positions
        std::cout << "\n=== Current Positions ===" << std::endl;
        auto positions = api.get_all_positions();
        if (positions.empty()) {
            std::cout << "No open positions" << std::endl;
        } else {
            for (const auto& pos : positions) {
                std::cout << pos.symbol << ": " 
                          << pos.qty << " shares @ $" 
                          << pos.current_price << " (P&L: $" 
                          << pos.unrealized_pl << ")" << std::endl;
            }
        }

        // Get all assets
        std::cout << "\n=== All Assets ===" << std::endl;
        auto assets = api.get_assets();
        for (const auto& asset : assets) {
            std::cout << asset.id << " " << asset.symbol << " - " << asset.name << " (" << asset.status << ") " << asset.asset_class << std::endl;
        }


    } catch (const alpaca::AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
