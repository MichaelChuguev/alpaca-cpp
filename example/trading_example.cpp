#include <iostream>
#include <thread>
#include <chrono>
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

        std::cout << "=== Trading Example ===" << std::endl;

        // 1. Create a limit order
        std::cout << "\n1. Creating a limit order for AAPL..." << std::endl;
        alpaca::Order limit_order(
            "AAPL",                              // symbol
            alpaca::Decimal(10),                  // quantity
            alpaca::OrderSide::BUY,              // side
            alpaca::OrderType::LIMIT,            // type
            alpaca::OrderTimeInForce::DAY        // time in force
        );
        limit_order.limit_price = alpaca::Decimal(150.00);  // set limit price
        
        auto order_response = api.create_order(limit_order);
        std::cout << "Order created with ID: " << order_response.id << std::endl;
        std::cout << "Status: " << order_response.status << std::endl;

        // 2. Get all orders
        std::cout << "\n2. Fetching all orders..." << std::endl;
        auto orders = api.get_orders(
            alpaca::OrderStatus::OPEN,
            10,
            alpaca::DateTime(),  // after
            alpaca::DateTime(),  // until  
            alpaca::Sort::ASC,
            false,  // nested
            std::vector<std::string>(),  // symbols
            alpaca::OrderSide::BUY
        );
        std::cout << "Found " << orders.size() << " open orders" << std::endl;

        // 3. Cancel the order we just created
        if (!order_response.id.empty()) {
            std::cout << "\n3. Cancelling order " << order_response.id << "..." << std::endl;
            api.delete_order(order_response.id);
            std::cout << "Order cancelled successfully" << std::endl;
        }

        // 4. Create a bracket order (buy with take-profit and stop-loss)
        std::cout << "\n4. Creating a bracket order..." << std::endl;
        alpaca::Order bracket_order(
            "TSLA",
            alpaca::Decimal(5),
            alpaca::OrderSide::BUY,
            alpaca::OrderType::MARKET,
            alpaca::OrderTimeInForce::DAY
        );
        bracket_order.order_class = alpaca::OrderClass::BRACKET;
        bracket_order.take_profit = {{"limit_price", alpaca::Decimal(450.00)}};  // Take profit above current price
        bracket_order.stop_loss = {{"stop_price", alpaca::Decimal(400.00)}};    // Stop loss below current price
        
        auto bracket_response = api.create_order(bracket_order);
        std::cout << "Bracket order created: " << bracket_response.id << std::endl;

        // Wait a moment and check order status
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        auto updated_order = api.get_order_by_id(bracket_response.id);
        std::cout << "Order status: " << updated_order.status << std::endl;

        // 5. Get portfolio history
        std::cout << "\n5. Fetching portfolio history..." << std::endl;
        auto portfolio = api.get_portfolio_history(
            "1D",  // period (use 1D for last day)
            "1Min",  // timeframe
            alpaca::IntradayReporting::CONTINUOUS,
            alpaca::DateTime(),  // start (empty when using period)
            alpaca::PnlReset::PER_DAY,
            alpaca::DateTime(),  // end (empty when using period)
            ""  // cashflow_types
        );
        std::cout << "Portfolio has " << portfolio.snapshots.size() 
                  << " data points" << std::endl;

    } catch (const alpaca::AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
