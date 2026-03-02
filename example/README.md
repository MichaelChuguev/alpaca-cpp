# Alpaca C++ Library Examples

This directory contains example programs demonstrating how to use the alpaca-cpp library.

## Examples

1. **simple_example.cpp** - Basic usage: Get account info, market clock, and positions
2. **trading_example.cpp** - Order management: Create, modify, and cancel orders
3. **watchlist_example.cpp** - Watchlist operations: Create, update, and manage watchlists
4. **crypto_example.cpp** - Crypto operations: View wallets, transfers, and whitelisted addresses

## Building the Examples

### Prerequisites

1. Set your Alpaca API credentials as environment variables:
```bash
export ALPACA_API_KEY="your_api_key_here"
export ALPACA_SECRET_KEY="your_secret_key_here"
```

2. Build the alpaca-cpp library first:
```bash
cd ..
mkdir -p build
cd build
cmake ..
cmake --build .
cd ../example
```

### Build Examples

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

This will create four executables:
- `simple_example`
- `trading_example`
- `watchlist_example`
- `crypto_example`

## Running the Examples

```bash
# From the example/build directory

# Run simple example (read-only operations)
./simple_example

# Run trading example (creates and cancels orders)
./trading_example

# Run watchlist example (creates and deletes watchlists)
./watchlist_example

# Run crypto example (views crypto wallets and transfers)
./crypto_example

# Run websocket example
./websocket_example
```

## Note

These examples use the **PAPER TRADING** endpoint by default. No real money or assets will be affected. To use live trading, change `alpaca::TraderAPIEndpoint::PAPER` to `alpaca::TraderAPIEndpoint::LIVE` in the source code (not recommended for testing!).

## API Credentials

You can get your API credentials from:
- Paper Trading: https://app.alpaca.markets/paper/dashboard/overview
- Live Trading: https://app.alpaca.markets/brokerage/dashboard/overview

Go to "Your API Keys" section to generate new keys.

## Troubleshooting

### "Library not found" error

If you get an error about alpaca-cpp not being found, make sure you've built the library first and that the `alpaca-cpp_DIR` path in CMakeLists.txt points to the correct build directory.

### "HTTP 401 Unauthorized" error

Check that your API keys are set correctly:
```bash
echo $ALPACA_API_KEY
echo $ALPACA_SECRET_KEY
```

### "Symbol not found" error

Make sure the symbols you're trading are valid and tradable. You can check using:
```cpp
auto asset = api.get_asset("AAPL");
std::cout << "Tradable: " << (asset.tradable ? "yes" : "no") << std::endl;
```
