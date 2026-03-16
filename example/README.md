# Examples

Runnable programs that demonstrate each major feature area of **alpaca-cpp**.

All examples use **paper trading** / **sandbox** endpoints by default — no real money is affected.

## List of examples

| Example | Description |
|---|---|
| `simple_example` | Account info, market clock, positions, assets |
| `trading_example` | Order management: create, modify, cancel |
| `watchlist_example` | Watchlist CRUD |
| `crypto_example` | Crypto wallets, transfers, whitelisted addresses |
| `websocket_example` | Real-time trade/quote streaming via WebSocket |
| `stock_data_example` | Historical and latest stock market data |
| `option_data_example` | Option bars, trades, quotes, snapshots, chain |
| `crypto_data_example` | Crypto bars, trades, quotes, orderbooks |
| `news_screener_example` | News articles and stock screener endpoints |
| `broker_sse_example` | Broker event streaming over SSE |
| `broker_get_accounts_example` | Broker account listing (legacy + client-credentials auth) |

## Building

From the repository root:

```bash
cmake -S . -B build -DALPACA_BUILD_EXAMPLES=ON
cmake --build build
```

## Credentials

Set the environment variables required by the example you want to run:

```bash
# Trading / Market Data
export ALPACA_API_KEY="your-api-key"
export ALPACA_SECRET_KEY="your-secret-key"

# Broker — legacy auth
export ALPACA_BROKER_API_KEY="your-broker-key"
export ALPACA_BROKER_API_SECRET="your-broker-secret"

# Broker — client credentials auth (recommended)
export ALPACA_BROKER_CLIENT_ID="your-client-id"
export ALPACA_BROKER_CLIENT_SECRET="your-client-secret"
```

Or copy `.env.example` to `.env`, fill in your keys, and `source .env`.

## Running

```bash
./build/example/simple_example
./build/example/trading_example
./build/example/websocket_example
./build/example/broker_sse_example
# ... etc.
```

## Getting API keys

- Paper Trading: <https://app.alpaca.markets/paper/dashboard/overview>
- Live Trading: <https://app.alpaca.markets/brokerage/dashboard/overview>

Navigate to the **API Keys** section to generate new keys.

## Troubleshooting

**401 Unauthorized** — verify you are using the correct endpoint (paper/live/sandbox) for your credentials and that env vars are exported in the current shell.

**Library not found** — make sure you built from the repository root with `-DALPACA_BUILD_EXAMPLES=ON` so the library target is available.
