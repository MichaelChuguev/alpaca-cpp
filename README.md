# alpaca-cpp

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)

A modern C++20 SDK for the [Alpaca](https://alpaca.markets) Trading, Market Data, Broker, and Auth APIs.

Contributions are welcome.

## Features

- **Strongly typed** request/response models for every endpoint.
- **REST clients** for Trading, Market Data, Broker, and Auth APIs.
- **Real-time streaming**:
  - Trade/order updates via WebSocket.
  - Market data (trades, quotes, bars) via WebSocket.
  - Broker events via Server-Sent Events (SSE).
- **Financial utility types**:
  - `Decimal` — fixed-point precision (8 decimal places) for prices and quantities.
  - `DateTime` — parsing/formatting for all Alpaca timestamp formats.
- **Broker API** with both legacy key/secret and OAuth2 client-credentials auth (automatic token refresh).
- **231 unit tests** covering model parsing, serialization, and URL/body construction.

## API clients

| Class | Description |
|---|---|
| `AlpacaTraderAPI` | Account, orders, positions, watchlists, activities, calendar/clock, options, treasuries, crypto wallets |
| `AlpacaMarketDataAPI` | Stocks, options, crypto, forex, fixed income, screener, news, logos, corporate actions |
| `AlpacaBrokerAPI` | Broker account lifecycle, account-scoped trading/funding, journals, reporting, rebalancing |
| `AlpacaAuthAPI` | OAuth2 token issuance (client credentials) |
| `AlpacaUpdatesStream` | Real-time trade/order updates (WebSocket) |
| `AlpacaMarketDataStream` | Real-time market data — trades, quotes, bars (WebSocket) |
| `BrokerSSEClient` | Broker event streams (SSE with auto-reconnect) |

## Quick start

### Requirements

- C++20 compiler (Clang 14+, GCC 12+, MSVC 2022+)
- CMake 3.16+
- OpenSSL
- [Howard Hinnant date](https://github.com/HowardHinnant/date)
- [nlohmann/json](https://github.com/nlohmann/json) (3.7.3+)
- [IXWebSocket](https://github.com/machinezone/IXWebSocket) (fetched automatically if not found)

### Install dependencies

macOS (Homebrew):

```bash
brew install cmake openssl howard-hinnant-date nlohmann-json
```

Ubuntu / Debian:

```bash
sudo apt install cmake libssl-dev nlohmann-json3-dev
```

> **Note:** Howard Hinnant `date` may need to be built from source on Linux — see the [CI workflow](.github/workflows/ci.yml) for an example.

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

| CMake option | Default | Description |
|---|---|---|
| `ALPACA_BUILD_TESTS` | `ON` | Build the GoogleTest suite |
| `ALPACA_BUILD_EXAMPLES` | `OFF` | Build example programs |

### Set up credentials

```bash
cp .env.example .env
# Edit .env with your keys, then:
source .env
```

At minimum for Trading + Market Data:

```bash
export ALPACA_API_KEY="your-api-key"
export ALPACA_SECRET_KEY="your-secret-key"
```

For the Broker API (choose one mode):

```bash
# Legacy key/secret auth
export ALPACA_BROKER_API_KEY="your-broker-key"
export ALPACA_BROKER_API_SECRET="your-broker-secret"

# Client credentials auth (recommended — automatic token refresh)
export ALPACA_BROKER_CLIENT_ID="your-client-id"
export ALPACA_BROKER_CLIENT_SECRET="your-client-secret"
```

### Run examples

```bash
cmake -S . -B build -DALPACA_BUILD_EXAMPLES=ON
cmake --build build

./build/example/simple_example
./build/example/stock_data_example
./build/example/websocket_example
./build/example/broker_get_accounts_example
```

See [`example/`](example/) for the full list.

## Usage

### Trading API

```cpp
#include <cstdlib>
#include <iostream>
#include "alpaca/AlpacaTraderAPI.h"

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
#include "alpaca/AlpacaMarketDataAPI.h"

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

### Broker API

Legacy auth:

```cpp
#include "alpaca/AlpacaBrokerAPI.h"

alpaca::AlpacaBrokerAPI broker(
    std::getenv("ALPACA_BROKER_API_KEY"),
    std::getenv("ALPACA_BROKER_API_SECRET"),
    alpaca::BrokerAPIEndpoint::SANDBOX
);

auto accounts = broker.get_accounts();
```

Client-credentials auth (recommended):

```cpp
#include "alpaca/AlpacaBrokerAPI.h"

alpaca::AlpacaBrokerAPI broker(
    std::getenv("ALPACA_BROKER_CLIENT_ID"),
    std::getenv("ALPACA_BROKER_CLIENT_SECRET"),
    alpaca::BrokerAPIEndpoint::SANDBOX,
    alpaca::BrokerAuthMode::CLIENT_CREDENTIALS
);

auto accounts = broker.get_accounts();
```

> In client-credentials mode, Bearer token retrieval and refresh are handled automatically inside the HTTP client. Use `AlpacaAuthAPI` if you need explicit access to the token payload.

### Streaming

| Stream | Transport | Example |
|---|---|---|
| Trade/order updates | Binary WebSocket | [`websocket_example.cpp`](example/websocket_example.cpp) |
| Market data | JSON WebSocket | [`websocket_example.cpp`](example/websocket_example.cpp) |
| Broker events | SSE (auto-reconnect) | [`broker_sse_example.cpp`](example/broker_sse_example.cpp) |

## Installation

Install to a prefix:

```bash
cmake --build build --target install
```

Consume from another CMake project:

```cmake
find_package(alpaca-cpp REQUIRED)
target_link_libraries(your_target PRIVATE alpaca::alpaca-cpp)
```

## Testing

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

cd build && ctest --output-on-failure
```

Run a subset:

```bash
./build/test/alpaca-tests --gtest_filter="TraderAPIURLTest.*"
```

## Project layout

```
include/alpaca/
  AlpacaTraderAPI.h          # Trading API client
  AlpacaMarketDataAPI.h      # Market Data API client
  AlpacaBrokerAPI.h          # Broker API client
  AlpacaAuthAPI.h            # Auth API client
  AlpacaUpdatesStream.h      # Trade updates WebSocket stream
  AlpacaMarketDataStream.h   # Market data WebSocket stream
  BrokerSSEClient.h          # Broker SSE event stream
  api/rest/                  # HTTP transport
  api/websocket/             # WebSocket transport
  core/                      # Decimal, DateTime, enums, errors, helpers
  model/                     # All typed request/response models
    trader/                  #   Trading models
    data/                    #   Market data models
    stream/                  #   Stream event models
    broker/                  #   Broker models

src/                         # Implementation files
example/                     # Runnable examples
test/                        # GoogleTest suites
cmake/                       # CMake package config templates
```

## Troubleshooting

**401 Unauthorized** — verify you are using the correct endpoint (paper / live / sandbox / production) for your credentials and that env vars are exported in the current shell.

**`find_package(alpaca-cpp)` not found** — install the library first (`cmake --build build --target install`) or point `CMAKE_PREFIX_PATH` to your install prefix.

**Stream examples fail to connect** — confirm network access and TLS trust store availability. For testing outside market hours, use the `TEST` feed in `websocket_example.cpp`.

## License

[MIT](LICENSE)
