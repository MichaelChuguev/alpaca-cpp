# alpaca-cpp

A modern C++20 client library for the [Alpaca](https://alpaca.markets/) trading API.

## Features

- **Trading API** — Full order lifecycle (create, modify, cancel, query), position management, account info
- **Market Data** — Real-time asset information, trading calendar, market clock
- **Multi-asset** — Stocks, options, crypto wallets & transfers, US treasuries
- **Authentication** — API key/secret and OAuth token support
- **Paper & Live** — Configurable endpoints for paper trading and live trading

## Requirements

- C++20 compiler (Clang 14+, GCC 12+, MSVC 2022+)
- CMake 3.16+
- [Boost](https://www.boost.org/) 1.70+
- [libcurl](https://curl.se/libcurl/)
- [OpenSSL](https://www.openssl.org/)
- [nlohmann/json](https://github.com/nlohmann/json) 3.7.3+

### macOS (Homebrew)

```bash
brew install cmake boost curl openssl nlohmann-json
```

### Ubuntu / Debian

```bash
sudo apt install cmake libboost-dev libcurl4-openssl-dev libssl-dev nlohmann-json3-dev
```

## Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### CMake options

| Option | Default | Description |
|---|---|---|
| `ALPACA_BUILD_TESTS` | `ON` | Build the test suite |
| `ALPACA_BUILD_EXAMPLES` | `OFF` | Build example programs |

## Installation

```bash
cmake --build build --target install
```

Then in your project's `CMakeLists.txt`:

```cmake
find_package(alpaca-cpp REQUIRED)
target_link_libraries(your_target PRIVATE alpaca::alpaca-cpp)
```

## Quick start

### Setting up credentials

1. Get your Trading API keys from [Alpaca Markets](https://app.alpaca.markets/paper/dashboard/overview)
2. Set up environment variables:

```bash
# Copy the example file
cp .env.example .env

# Edit .env with your actual credentials
# Then source it:
source .env
```

Or export them directly:

```bash
export ALPACA_API_KEY="your-api-key"
export ALPACA_SECRET_KEY="your-secret-key"
```

### Example usage

```cpp
#include <alpaca/alpacaAPI.h>
#include <alpaca/api/rest/trader/AlpacaTraderAPI.h>

int main() {
    // Use environment variables for credentials
    const char* key = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");

    alpaca::AlpacaTraderAPI api(key, secret, alpaca::TraderAPIEndpoint::PAPER);

    // Get account info
    auto account = api.get_account();
    std::cout << account.to_string() << std::endl;

    // Create a market order
    alpaca::Order order("AAPL", 10, alpaca::BUY, alpaca::MARKET, alpaca::DAY);
    auto response = api.create_order(order);
    std::cout << "Order ID: " << response.id << std::endl;

    return 0;
}
```

## API Reference

### Account & Configuration
| Method | Description |
|---|---|
| `get_account()` | Get account information |
| `get_account_config()` | Get account configuration |
| `send_account_config()` | Update account configuration |

### Orders
| Method | Description |
|---|---|
| `create_order()` | Submit a new order |
| `get_orders()` | Get all orders (with filters) |
| `get_order_by_id()` | Get a specific order |
| `get_order_by_client_order_id()` | Get order by client ID |
| `replace_order()` | Replace an existing order |
| `delete_order()` | Cancel an order |
| `delete_all_orders()` | Cancel all orders |

### Positions
| Method | Description |
|---|---|
| `get_all_positions()` | Get all open positions |
| `get_open_position()` | Get a specific position |
| `close_position()` | Close a position |
| `close_all_positions()` | Close all positions |

### Assets
| Method | Description |
|---|---|
| `get_assets()` | Get assets (with filters) |
| `get_asset()` | Get a specific asset |
| `get_option_contracts()` | Get option contracts |
| `get_option_contract()` | Get a specific option contract |
| `exercise_option()` | Exercise an option |
| `do_not_exercise_option()` | Submit DNE instruction |
| `get_us_treasuries()` | Get US treasury securities |

### Watchlists
| Method | Description |
|---|---|
| `get_all_watchlists()` | Get all watchlists |
| `create_watchlist()` | Create a watchlist |
| `get_watchlist_by_id()` | Get watchlist by ID |
| `get_watchlist_by_name()` | Get watchlist by name |
| `update_watchlist()` | Update a watchlist |
| `add_asset_to_watchlist()` | Add asset to watchlist |
| `remove_asset_from_watchlist()` | Remove asset from watchlist |
| `delete_watchlist()` | Delete a watchlist |

### Account Activities
| Method | Description |
|---|---|
| `get_account_activities()` | Get account activities |
| `get_account_activities_by_type()` | Get activities by type |

### Calendar & Clock
| Method | Description |
|---|---|
| `get_calendar()` | Get market calendar |
| `get_clock()` | Get market clock |
| `get_portfolio_history()` | Get portfolio history |

### Crypto Wallets
| Method | Description |
|---|---|
| `get_crypto_wallets()` | Get crypto wallets |
| `get_crypto_transfers()` | Get crypto transfers |
| `create_crypto_transfer()` | Create a crypto transfer |
| `get_crypto_transfer()` | Get a specific transfer |
| `get_whitelisted_addresses()` | Get whitelisted addresses |
| `create_whitelisted_address()` | Add a whitelisted address |
| `delete_whitelisted_address()` | Remove a whitelisted address |
| `get_crypto_transfer_estimate()` | Estimate transfer fees |

## Testing

The project includes 190 comprehensive tests covering all Trading API endpoints and WebSocket streams.

### Running tests

```bash
# Build the project with tests enabled (default)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Run all tests
./build/test/alpaca-tests

# Run with CTest
cd build && ctest --output-on-failure

# Run specific test suite
./build/test/alpaca-tests --gtest_filter="OrderTest.*"
```

### Test coverage

- **Model tests**: Account, Order, Position, Portfolio, Asset, Watchlist, Activity, Calendar, Clock, Option, Treasury, Wallet
- **API tests**: URL construction, JSON body building, response parsing, enum mappings  
- **Stream tests**: StreamTrade, StreamQuote, StreamBar, TradeUpdate, WebSocket protocol messages
- **47 Trading API endpoints** fully tested across 11 categories

Each test includes console output showing endpoint, request/response data, and validation status.

## Project structure

```
include/alpaca/         Public headers
  alpacaAPI.h           Main entry point
  api/rest/             REST client & endpoint interfaces
  core/                 Types, error handling, utilities
  model/trader/         Data models (Account, Order, Position, …)
src/                    Implementation files
test/                   Test suite (Google Test)
example/                Example programs
cmake/                  CMake package config
```

## Examples

See the `example/` directory for complete working examples:

- **simple_example.cpp** — Basic trading operations
- **trading_example.cpp** — Advanced order management
- **watchlist_example.cpp** — Watchlist operations
- **crypto_example.cpp** — Crypto wallet and transfers
- **websocket_example.cpp** — Real-time WebSocket streams (market data & trade updates)

Build examples:

```bash
cmake -S . -B build -DALPACA_BUILD_EXAMPLES=ON
cmake --build build
cd example/build && ./simple_example
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Add tests for new functionality
4. Ensure all tests pass (`./build/test/alpaca-tests`)
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## License

[MIT](LICENSE)
