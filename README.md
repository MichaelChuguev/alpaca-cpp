# alpaca-cpp

A modern C++20 client library for the [Alpaca](https://alpaca.markets/) Trading and Market Data APIs.

Contributions welcome.

## Features

- **Authentication** — Trading key/secret, Trading OAuth token, Broker legacy key/secret headers, and Broker client-credentials (automatic Bearer token refresh)
- **Paper & Live** — Configurable endpoints for paper trading and live trading
- **Trading API REST** — Full order lifecycle (create, modify, cancel, query), position management, account info, watchlists, portfolio history, crypto wallets, market clock & calendar
- **Market Data API REST** — Stocks, options, crypto, forex, fixed income, news, screener, corporate actions, logos
- **Broker API REST** — Accounts, documents, account-level trading, funding/ACH, journals, rebalancing, reporting, SSE subscriptions, and account-scoped crypto funding
- **Auth API REST** — OAuth2 client-credentials token issuance helper
- **Trading API WebSockets** — Order lifecycle updates, trade execution updates, account activity
- **Market Data API WebSockets** — Real-time trades, quotes, and bars for stocks, options, and crypto
- **Custom Types** — Fixed-point `Decimal` (8 decimal places) for precise financial math; ISO 8601 `DateTime` with timezone support

## Requirements

- C++20 compiler (Clang 14+, GCC 12+, MSVC 2022+)
- CMake 3.16+
- [IXWebSocket](https://github.com/machinezone/IXWebSocket)
- [OpenSSL](https://www.openssl.org/)
- [Howard Hinnant date](https://github.com/HowardHinnant/date)
- [nlohmann/json](https://github.com/nlohmann/json) 3.7.3+

### macOS (Homebrew)

```bash
brew install cmake openssl howard-hinnant-date nlohmann-json
```

### Ubuntu / Debian

```bash
sudo apt install cmake libssl-dev libdate-dev nlohmann-json3-dev
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

1. Get your API keys from [Alpaca Markets](https://alpaca.markets)
2. Set up environment variables:

```bash
# Copy the example
cp .env.example .env

# Edit .env:
source .env
```

Or export them directly:

```bash
export ALPACA_API_KEY="your-api-key"
export ALPACA_SECRET_KEY="your-secret-key"
```

### Trading API example

```cpp
#include <alpaca/AlpacaAPI.h>
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
    alpaca::Order order("AAPL", alpaca::Decimal(10),
                        alpaca::OrderSide::BUY,
                        alpaca::OrderType::MARKET,
                        alpaca::OrderTimeInForce::DAY);
    auto response = api.create_order(order);
    std::cout << "Order ID: " << response.id << std::endl;

    return 0;
}
```

### Broker API authentication modes

The Broker API supports both Alpaca authentication flows:

1. **Legacy flow** (direct headers): `APCA-API-KEY-ID` + `APCA-API-SECRET-KEY`
2. **Client credentials flow** (recommended): `client_id` + `client_secret` exchanged for short-lived Bearer tokens

The client-credentials token exchange is AUTOMATIC (including refresh before expiry), so you do **not** fetch tokens manually for Broker calls.
(This may change in the future)

#### Broker API (legacy)

```cpp
#include <alpaca/api/rest/broker/AlpacaBrokerAPI.h>

int main() {
  alpaca::AlpacaBrokerAPI broker(
    "YOUR_BROKER_KEY_ID",
    "YOUR_BROKER_SECRET_KEY",
    alpaca::BrokerAPIEndpoint::SANDBOX
  );

  auto accounts = broker.get_accounts();
  std::cout << "accounts: " << accounts.size() << std::endl;
}
```

#### Broker API (client credentials, auto token management)

```cpp
#include <alpaca/api/rest/broker/AlpacaBrokerAPI.h>

int main() {
  alpaca::AlpacaBrokerAPI broker(
    "YOUR_CLIENT_ID",
    "YOUR_CLIENT_SECRET",
    alpaca::BrokerAPIEndpoint::SANDBOX,
    alpaca::BrokerAuthMode::CLIENT_CREDENTIALS
  );

  // SDK exchanges credentials for a Bearer token and refreshes automatically.
  auto accounts = broker.get_accounts();
  std::cout << "accounts: " << accounts.size() << std::endl;
}
```

### Auth API example (explicit token issuance)

Use this when you need direct access to token response payloads.

```cpp
#include <alpaca/api/rest/auth/AlpacaAuthAPI.h>

int main() {
  alpaca::AlpacaAuthAPI auth(
    "YOUR_CLIENT_ID",
    "YOUR_CLIENT_SECRET",
    alpaca::AuthAPIEndpoint::SANDBOX
  );

  auto token = auth.issue_tokens(); // grant_type=client_credentials by default
  std::cout << token.token_type << " " << token.access_token << std::endl;
  std::cout << "expires_in: " << token.expires_in << std::endl;
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

### Broker API (selected methods)
| Method | Description |
|---|---|
| `get_accounts()` | List broker accounts |
| `create_account()` | Create a new broker account |
| `get_account()` | Get a broker account by ID |
| `patch_account()` | Partially update a broker account |
| `get_trading_account()` | Get trading account object for broker account |
| `get_all_orders_for_account()` | List orders for an account |
| `create_order_for_account()` | Submit an order for an account |
| `get_positions_for_account()` | List positions for an account |
| `close_all_positions_for_account()` | Liquidate all positions for an account |
| `get_account_ach_relationships()` | List ACH relationships |
| `create_transfer_for_account()` | Create account transfer |
| `get_all_journals()` | List journals |
| `subscribe_to_account_status_sse()` | Build account-status SSE subscription |
| `create_account_status_sse_client()` | Create ready-to-run account-status SSE client |

### Auth API
| Method | Description |
|---|---|
| `issue_tokens()` | Exchange client credentials (or assertion) for OAuth2 token |

### Market Data — Stocks
| Method | Description |
|---|---|
| `get_stock_trades()` | Historical trades |
| `get_stock_trades_latest()` | Latest trade for one or more symbols |
| `get_stock_quotes()` | Historical quotes |
| `get_stock_quotes_latest()` | Latest quote for one or more symbols |
| `get_stock_bars()` | Historical bars (OHLCV) |
| `get_stock_auctions()` | Auction data |
| `get_stock_snapshots()` | Current snapshot for one or more symbols |
| `get_stock_conditions()` | Condition code mappings |
| `get_stock_exchange_codes()` | Exchange code mappings |

### Market Data — Options
| Method | Description |
|---|---|
| `get_option_trades()` | Historical option trades |
| `get_option_trades_latest()` | Latest option trade |
| `get_option_quotes()` | Historical option quotes |
| `get_option_quotes_latest()` | Latest option quote |
| `get_option_bars()` | Historical option bars |
| `get_option_snapshots()` | Option snapshots |
| `get_option_exchange_codes()` | Option exchange codes |
| `get_option_conditions()` | Option condition codes |

### Market Data — Crypto
| Method | Description |
|---|---|
| `get_crypto_trades()` | Historical crypto trades |
| `get_crypto_trades_latest()` | Latest crypto trade |
| `get_crypto_quotes()` | Historical crypto quotes |
| `get_crypto_quotes_latest()` | Latest crypto quote |
| `get_crypto_bars()` | Historical crypto bars |
| `get_crypto_snapshots()` | Crypto snapshots |
| `get_crypto_orderbooks()` | Crypto order book data |

### Market Data — News, Screener & More
| Method | Description |
|---|---|
| `get_news()` | News articles with optional filters |
| `get_most_actives()` | Most active stocks/crypto |
| `get_movers()` | Top movers by market type |
| `get_forex_rates()` | Forex exchange rates |
| `get_fixed_income_prices()` | Fixed income pricing |
| `get_corporate_actions()` | Corporate action events |
| `get_logo()` | Company/crypto logo |

## Testing

231 basic unit tests covering all model parsing, serialization, and API URL/body construction.

More to be done soon

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

## Project structure

```
include/alpaca/         Public headers
  AlpacaAPI.h           Main entry point (facade for all sub-APIs)
  api/rest/             REST client & endpoint interfaces
    auth/               Auth API endpoints (token issuance)
    broker/             Broker API endpoints
    trader/             Trading API endpoints
    marketdata/         Market Data API endpoints
  api/sse/              Broker Server-Sent Events (SSE) client
  api/websocket/        WebSocket client & streams
  core/                 Types (Decimal, DateTime), error handling, utilities
  model/trader/         Trading data models (Account, Order, Position, …)
  model/data/           Market data models (Stock, Option, Crypto, News, …)
  model/stream/         WebSocket stream models
src/                    Implementation files
test/                   Test suite (Google Test, 231 tests)
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
- **stock_data_example.cpp** — Historical and latest stock market data
- **option_data_example.cpp** — Option chain data and snapshots
- **crypto_data_example.cpp** — Crypto market data
- **news_screener_example.cpp** — News articles and market screener

Build and run examples:

```bash
cmake -S . -B build -DALPACA_BUILD_EXAMPLES=ON
cmake --build build
./build/example/simple_example
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch 
3. ADD TESTS for new functionality
4. Ensure ALL tests pass (`./build/test/alpaca-tests`)
5. Commit your changes 
6. Push to the branch 
7. Open a Pull Request

## License

[MIT](LICENSE)
