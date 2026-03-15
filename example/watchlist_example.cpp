#include <iostream>
#include "alpaca/AlpacaTraderAPI.h"
#include "alpaca/core/error.h"

int main() {
    const char* key = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");
    
    if (!key || !secret) {
        std::cerr << "Please set ALPACA_API_KEY and ALPACA_SECRET_KEY environment variables" << std::endl;
        return 1;
    }

    try {
        alpaca::AlpacaTraderAPI api(key, secret, alpaca::TraderAPIEndpoint::PAPER);

        std::cout << "=== Watchlist Example ===" << std::endl;

        // 1. Get all existing watchlists
        std::cout << "\n1. Getting all watchlists..." << std::endl;
        auto watchlists = api.get_all_watchlists();
        std::cout << "Found " << watchlists.size() << " watchlists" << std::endl;
        for (const auto& wl : watchlists) {
            std::cout << "  - " << wl.name << " (ID: " << wl.id << ")" << std::endl;
        }

        // Delete "Tech Stocks" if it already exists (from previous run)
        for (const auto& wl : watchlists) {
            if (wl.name == "Tech Stocks") {
                std::cout << "\nDeleting existing 'Tech Stocks' watchlist..." << std::endl;
                api.delete_watchlist(wl.id);
                break;
            }
        }

        // 2. Create a new watchlist
        std::cout << "\n2. Creating a new watchlist 'Tech Stocks'..." << std::endl;
        std::vector<std::string> tech_symbols = {"AAPL", "GOOGL", "MSFT"};
        
        auto created_watchlist = api.create_watchlist("Tech Stocks", tech_symbols);
        std::cout << "Created watchlist ID: " << created_watchlist.id << std::endl;
        std::cout << "Symbols: ";
        for (const auto& asset : created_watchlist.assets) {
            std::cout << asset.symbol << " ";
        }
        std::cout << std::endl;

        // 3. Add a symbol to the watchlist
        std::cout << "\n3. Adding NVDA to the watchlist..." << std::endl;
        auto updated_watchlist = api.add_asset_to_watchlist(created_watchlist.id, "NVDA");
        std::cout << "Updated watchlist now has " << updated_watchlist.assets.size() 
                  << " symbols" << std::endl;

        // 4. Get watchlist by name
        std::cout << "\n4. Getting watchlist by name..." << std::endl;
        auto fetched_watchlist = api.get_watchlist_by_name("Tech Stocks");
        std::cout << "Watchlist '" << fetched_watchlist.name << "' contains:" << std::endl;
        for (const auto& asset : fetched_watchlist.assets) {
            std::cout << "  - " << asset.symbol << " (" << asset.name << ")" << std::endl;
        }

        // 5. Update watchlist name
        std::cout << "\n5. Renaming watchlist..." << std::endl;
        std::vector<std::string> updated_symbols = {"AAPL", "GOOGL", "MSFT", "NVDA", "TSLA"};
        
        auto renamed_watchlist = api.update_watchlist(created_watchlist.id, "Top Tech Stocks", updated_symbols);
        std::cout << "Watchlist renamed to: " << renamed_watchlist.name << std::endl;

        // 6. Delete the watchlist
        std::cout << "\n6. Deleting watchlist..." << std::endl;
        api.delete_watchlist(created_watchlist.id);
        std::cout << "Watchlist deleted successfully" << std::endl;

    } catch (const alpaca::AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
