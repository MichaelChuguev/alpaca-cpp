# alpaca-cpp

A modern C++20 SDK for Alpaca Trading, Market Data, Broker, and Auth APIs.

Contributions are welcome.

## Why this library

- C++20-first API design with strongly typed request/response models.
- REST clients for trading, market data, broker, and auth endpoints.
- Real-time streaming support:
  - Trading updates over WebSocket.
  - Market data over WebSocket.
  - Broker events over Server-Sent Events (SSE).
- Financial utility types:
  - `Decimal` fixed-point precision (8 decimal places).
  - `DateTime` parsing/formatting for Alpaca timestamp formats.

## What is included

- `AlpacaTraderAPI` for account, orders, positions, watchlists, activities, calendar/clock, options, treasuries, and crypto wallet endpoints.
- `AlpacaMarketDataAPI` for stocks, options, crypto, forex, fixed income, screener, news, logos, and corporate actions.
- `AlpacaBrokerAPI` for broker account lifecycle and account-scoped trading/funding operations.
- `AlpacaAuthAPI` for explicit OAuth2 token issuance (client credentials).
- `AlpacaUpdatesStream` for trade/order updates.
- `AlpacaMarketDataStream` for live market trades/quotes/bars.
- `BrokerSSEClient` for broker event streams.

## Quick Start

### 1) Requirements

- C++20 compiler (Clang 14+, GCC 12+, MSVC 2022+)
- CMake 3.16+
- OpenSSL
- Howard Hinnant date
- nlohmann/json (3.7.3+)
- IXWebSocket

Notes:
- IXWebSocket is resolved with `find_package(ixwebsocket)` first.
- If not found, CMake fetches IXWebSocket automatically via `FetchContent`.

### 2) Install dependencies

macOS (Homebrew):

```bash
brew install cmake openssl howard-hinnant-date nlohmann-json
```

Ubuntu / Debian:

```bash
sudo apt install cmake libssl-dev libdate-dev nlohmann-json3-dev
```

### 3) Configure and build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Useful CMake options:

| Option | Default | Description |
|---|---|---|
| `ALPACA_BUILD_TESTS` | `ON` | Build GoogleTest suite |
| `ALPACA_BUILD_EXAMPLES` | `OFF` | Build examples under `example/` |

### 4) Configure credentials

```bash
cp .env.example .env
source .env
```

At minimum for Trading + Market Data examples:

```bash
export ALPACA_API_KEY="your-api-key"
export ALPACA_SECRET_KEY="your-secret-key"
```

Broker credentials (choose one auth mode):

```bash
# Legacy headers auth
export ALPACA_BROKER_API_KEY="your-broker-key"
export ALPACA_BROKER_API_SECRET="your-broker-secret"

# Client credentials auth (recommended)
export ALPACA_BROKER_CLIENT_ID="your-client-id"
export ALPACA_BROKER_CLIENT_SECRET="your-client-secret"
```

### 5) Build and run examples

```bash
cmake -S . -B build -DALPACA_BUILD_EXAMPLES=ON
cmake --build build

./build/example/simple_example
./build/example/stock_data_example
./build/example/websocket_example
./build/example/broker_sse_example
```

## Installation and CMake package usage

Install to your prefix:

```bash
cmake --build build --target install
```

Consume from another CMake project:

```cmake
find_package(alpaca-cpp REQUIRED)
target_link_libraries(your_target PRIVATE alpaca::alpaca-cpp)
```

## Usage Guide

### Trading API

```cpp
#include <cstdlib>
#include <iostream>
#include "alpaca/api/rest/trader/AlpacaTraderAPI.h"

int main() {
    const char* key = std::getenv("ALPACA_API_KEY");
    const char* secret = std::getenv("ALPACA_SECRET_KEY");
    if (!key || !secret) return 1;

    alpaca::AlpacaTraderAPI api(key, secret, alpaca::TraderAPIEndpoint::PAPER);
    auto account = api.get_account();
    std::cout << "Account: " << account.id << "\n";
}
```

### Market Data API

```cpp
#include "alpaca/api/rest/marketdata/AlpacaMarketDataAPI.h"

int main() {
    alpaca::AlpacaMarketDataAPI md(
        std::getenv("ALPACA_API_KEY"),
        std::getenv("ALPACA_SECRET_KEY"),
        alpaca::MarketDataEndpoint::LIVE,
        alpaca::DataFeed::IEX
    );

    auto latest = md.get_stock_latest_quotes({"AAPL", "MSFT"});
}
```

### Broker API auth modes

Legacy auth:

```cpp
#include "alpaca/api/rest/broker/AlpacaBrokerAPI.h"

alpaca::AlpacaBrokerAPI broker(
    std::getenv("ALPACA_BROKER_API_KEY"),
    std::getenv("ALPACA_BROKER_API_SECRET"),
    alpaca::BrokerAPIEndpoint::SANDBOX
);
```

Client-credentials auth (recommended):

```cpp
#include "alpaca/api/rest/broker/AlpacaBrokerAPI.h"

alpaca::AlpacaBrokerAPI broker(
    std::getenv("ALPACA_BROKER_CLIENT_ID"),
    std::getenv("ALPACA_BROKER_CLIENT_SECRET"),
    alpaca::BrokerAPIEndpoint::SANDBOX,
    alpaca::BrokerAuthMode::CLIENT_CREDENTIALS
);
```

Notes:
- In client-credentials mode, Bearer token retrieval and refresh are handled automatically inside the HTTP client.
- Use `AlpacaAuthAPI` if you need explicit token payload access.

### Streaming APIs

- Trading updates stream: `AlpacaUpdatesStream` (binary WS frames).
- Market data stream: `AlpacaMarketDataStream` (JSON text WS frames).
- Broker events stream: `BrokerSSEClient` (SSE with reconnect support).

See the runnable examples:
- `example/websocket_example.cpp`
- `example/broker_sse_example.cpp`

## API Surface At a Glance

Public entry headers:

- `include/alpaca/api/rest/trader/AlpacaTraderAPI.h`
- `include/alpaca/api/rest/marketdata/AlpacaMarketDataAPI.h`
- `include/alpaca/api/rest/broker/AlpacaBrokerAPI.h`
- `include/alpaca/api/rest/auth/AlpacaAuthAPI.h`
- `include/alpaca/api/websocket/AlpacaUpdatesStream.h`
- `include/alpaca/api/websocket/AlpacaMarketDataStream.h`
- `include/alpaca/api/sse/BrokerSSEClient.h`
- `include/alpaca/core/types.h`

Trader API categories:
- Account/config
- Assets/options/treasuries
- Orders/positions/portfolio history
- Watchlists and account activities
- Calendar/clock
- Crypto wallets/transfers/whitelist

Market Data API categories:
- Stocks (trades/quotes/bars/auctions/snapshots/meta)
- Options (bars/trades/latest quotes/snapshots/chain/meta)
- Crypto (bars/trades/quotes/orderbooks/snapshots)
- Forex, fixed income, logos
- Screener, news, corporate actions

Broker API coverage:
- Account lifecycle and documents
- Account-scoped trading and funding
- Journals, reporting, rebalancing, SSE subscriptions
- Account-scoped crypto funding wallet flows

## Testing

This repo includes 231 GoogleTest unit tests covering model parsing/serialization and a large set of URL/body construction paths.

Build and run tests:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

./build/test/alpaca-tests
cd build && ctest --output-on-failure
```

Run a subset:

```bash
./build/test/alpaca-tests --gtest_filter="TraderAPIURLTest.*"
```

## Project Layout

```text
include/alpaca/
  api/
    rest/          # Trader, MarketData, Broker, Auth REST clients
    websocket/     # Trading + market-data stream clients
    sse/           # Broker SSE client
  core/            # Decimal, DateTime, enums, errors, helpers
  model/           # Trader/data/stream/broker model types

src/
  api/rest/        # HTTP client implementation
  api/websocket/   # WebSocket stream implementations
  api/sse/         # SSE implementation
  trader/          # Trader API implementation
  marketdata/      # Market data API implementation
  broker/          # Broker API implementation
  auth/            # Auth API implementation

example/           # End-to-end runnable examples
test/              # GoogleTest suites
cmake/             # Package config templates
```

## Migration Note

- The old entrypoint (`AlpacaAPI.h` / `AlpacaAPI.cpp`) has been removed.
- Use specialized APIs directly (`AlpacaTraderAPI`, `AlpacaMarketDataAPI`, `AlpacaBrokerAPI`, streams).

## Troubleshooting

`401 Unauthorized`:
- Verify you are using the correct endpoint (paper/live/sandbox/production) for your credentials.
- Check env vars are exported in the current shell.

`find_package(alpaca-cpp)` not found:
- Install the library (`cmake --build build --target install`) or point CMake to your build/install prefix.

Examples fail to connect:
- Confirm your network and TLS trust store are available.
- For stream testing outside market hours, use the `TEST` market-data feed in `websocket_example.cpp`.

## Contributing

1. Fork the repository.
2. Create a feature branch.
3. Add or update tests for your changes.
4. Ensure tests pass.
5. Open a pull request.

## License

[MIT](LICENSE)
