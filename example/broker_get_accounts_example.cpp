#include <cstdlib>
#include <iostream>

#include "alpaca/AlpacaBrokerAPI.h"

int main() {
    const char* key    = std::getenv("ALPACA_BROKER_API_KEY");
    const char* secret = std::getenv("ALPACA_BROKER_API_SECRET");
    const char* cc_id  = std::getenv("ALPACA_BROKER_CLIENT_ID");
    const char* cc_sec = std::getenv("ALPACA_BROKER_CLIENT_SECRET");

    const bool use_cc = cc_id && cc_sec;

    if (!use_cc && (!key || !secret)) {
        std::cerr << "Set either:\n"
                  << "  ALPACA_BROKER_API_KEY + ALPACA_BROKER_API_SECRET  (legacy)\n"
                  << "  ALPACA_BROKER_CLIENT_ID + ALPACA_BROKER_CLIENT_SECRET  (client credentials)\n";
        return 1;
    }

    try {
        std::unique_ptr<alpaca::AlpacaBrokerAPI> broker_ptr;
        if (use_cc) {
            std::cout << "[auth] Using client-credentials flow\n";
            broker_ptr = std::make_unique<alpaca::AlpacaBrokerAPI>(
                cc_id, cc_sec,
                alpaca::BrokerAPIEndpoint::SANDBOX,
                alpaca::BrokerAuthMode::CLIENT_CREDENTIALS);
        } else {
            std::cout << "[auth] Using legacy key/secret flow\n";
            broker_ptr = std::make_unique<alpaca::AlpacaBrokerAPI>(
                key, secret, alpaca::BrokerAPIEndpoint::SANDBOX);
        }

        auto accounts = broker_ptr->get_accounts();

        std::cout << "Total accounts: " << accounts.size() << "\n\n";
        for (const auto& account : accounts) {
            std::cout << account.to_string() << "\n";
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
