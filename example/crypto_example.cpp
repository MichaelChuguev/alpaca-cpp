#include <iostream>
#include "alpaca/alpacaAPI.h"
#include "alpaca/api/rest/trader/AlpacaTraderAPI.h"
#include "alpaca/core/error.h"

int main() {
    const char* key = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");
    
    if (!key || !secret) {
        std::cerr << "Please set ALPACA_API_KEY and ALPACA_SECRET_KEY environment variables" << std::endl;
        return 1;
    }

    std::cout << "=== Crypto Wallet Example ===" << std::endl;
    std::cout << "\nNote: Crypto endpoints are only available for live, crypto-enabled accounts." << std::endl;
    std::cout << "This example may not work with paper trading accounts.\n" << std::endl;

    try {
        alpaca::AlpacaTraderAPI api(key, secret, alpaca::TraderAPIEndpoint::PAPER);

        // 1. Get all crypto wallets
        std::cout << "1. Getting crypto wallets..." << std::endl;
        auto wallets = api.get_crypto_wallets();
        std::cout << "Found " << wallets.size() << " crypto wallets" << std::endl;
        for (const auto& wallet : wallets) {
            std::cout << "  - " << wallet.asset << ": " 
                      << wallet.qty << " (available: " 
                      << wallet.available_qty << ")" << std::endl;
        }

        // 2. Get crypto transfer history
        std::cout << "\n2. Getting crypto transfers..." << std::endl;
        auto transfers = api.get_crypto_transfers();
        std::cout << "Found " << transfers.size() << " transfers" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), transfers.size()); ++i) {
            const auto& transfer = transfers[i];
            std::cout << "  - " << transfer.asset << " " 
                      << transfer.amount << " (" 
                      << transfer.status << ")" << std::endl;
        }

        // 3. Get whitelisted addresses
        std::cout << "\n3. Getting whitelisted addresses..." << std::endl;
        auto addresses = api.get_whitelisted_addresses();
        std::cout << "Found " << addresses.size() << " whitelisted addresses" << std::endl;
        for (const auto& addr : addresses) {
            std::cout << "  - " << addr.asset << ": " 
                      << addr.address << " (" 
                      << addr.status << ")" << std::endl;
        }

        // 4. Estimate a crypto transfer (example - adjust parameters as needed)
        std::cout << "\n4. Estimating a crypto transfer..." << std::endl;
        try {
            auto estimate = api.get_crypto_transfer_estimate("BTC", "0.001");
            std::cout << "Estimated fee: " << estimate.fee << " " << estimate.asset << std::endl;
        } catch (const alpaca::AlpacaTraderException& e) {
            std::cout << "Could not get estimate (no crypto account or insufficient balance)" << std::endl;
        }

        // 5. Example: Create a whitelisted address (commented out to avoid actual changes)
        /*
        std::cout << "\n5. Creating a whitelisted address..." << std::endl;
        alpaca::WhitelistedAddressRequest addr_request;
        addr_request.asset = "BTC";
        addr_request.address = "your-bitcoin-address-here";
        
        auto whitelisted = api.create_whitelisted_address(addr_request);
        std::cout << "Whitelisted address created: " << whitelisted.id << std::endl;
        */

        // 6. Example: Create a crypto transfer (commented out to avoid actual transfers)
        /*
        std::cout << "\n6. Creating a crypto transfer..." << std::endl;
        alpaca::CryptoTransferRequest transfer_request;
        transfer_request.asset = "BTC";
        transfer_request.amount = 0.001;
        transfer_request.address = "recipient-address";
        
        auto transfer = api.create_crypto_transfer(transfer_request);
        std::cout << "Transfer created with ID: " << transfer.id << std::endl;
        std::cout << "Status: " << transfer.status << std::endl;
        */

    } catch (const std::runtime_error& e) {
        std::string error_msg(e.what());
        if (error_msg.find("404") != std::string::npos || error_msg.find("endpoint not found") != std::string::npos) {
            std::cerr << "\nError: Crypto endpoints not available on this account." << std::endl;
            std::cerr << "Crypto features require a live, crypto-enabled account." << std::endl;
        } else {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        return 1;
    } catch (const alpaca::AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
