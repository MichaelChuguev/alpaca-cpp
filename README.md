# alpaca-cpp

A modern C++20 client library for the [Alpaca](https://alpaca.markets/) trading API.

## Features

- **Trading API** — Full order lifecycle (create, modify, cancel, query), position management, account info
- **Market Data** — Real-time asset information, trading calendar, market clock
- **Multi-asset** — Stocks, options, with crypto-ready infrastructure
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

```cpp
#include <alpaca/alpacaAPI.h>

int main() {
    // Paper trading with API key authentication
    alpaca::AlpacaAPI api(
        "YOUR_KEY_ID",
        "YOUR_SECRET_KEY",
        alpaca::TraderAPIEndpoint::PAPER,
        alpaca::MarketDataWebsocketSource::IEX
    );

    // Or use OAuth
    // alpaca::AlpacaAPI api("OAUTH_TOKEN", alpaca::TraderAPIEndpoint::PAPER);
}
```

## Project structure

```
include/alpaca/         Public headers
  alpacaAPI.h           Main entry point
  api/rest/             REST client & endpoint interfaces
  api/websocket/        WebSocket client (planned)
  core/                 Types, error handling
  model/trader/         Data models (Account, Order, Position, …)
  service/              High-level services (planned)
src/                    Implementation files
test/                   Test suite
cmake/                  CMake package config
```

## License

[MIT](LICENSE) — Michael Chuguev
