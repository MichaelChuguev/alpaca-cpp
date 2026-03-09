#pragma once

#include <string>
#include <memory>

#include "alpaca/api/rest/HTTPClient.h"
#include "alpaca/core/types.h"
#include "alpaca/api/rest/trader/AlpacaTraderAPI.h"
#include "alpaca/api/rest/marketdata/AlpacaMarketDataAPI.h"
#include "alpaca/api/websocket/AlpacaUpdatesStream.h"
#include "alpaca/api/websocket/AlpacaMarketDataStream.h"

namespace alpaca {


class AlpacaAPI {

    private:

    const std::string traderKeyID;
    const std::string traderKeySecret;
    const std::string traderOAuthToken;
    const TraderAPIEndpoint traderAPIEndpoint;
    const MarketDataWebsocketSource marketDataWebsocketSource;
    const MarketDataEndpoint marketDataEndpoint;

    const std::string brokerAPIKey;
    const std::string brokerAPISecret;
    const BrokerAPIEndpoint brokerAPIEndpoint;

    //AlpacaBrokerAPI broker;


    public:

    /**
     * Instantiates a new AlpacaAPI. Use this constructor if you are using the Trading or Market Data APIs for a
     * single Alpaca account.
     *
     * @param traderKeyID                   the Trader key ID
     * @param traderSecretKey               the Trader secret key
     * @param traderAPIEndpoint             the TraderAPIEndpoint
     * @param marketDataWebsocketSource     the MarketDataWebsocketSource
     */
    AlpacaAPI(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint, const MarketDataWebsocketSource& marketDataWebsocketSource);
    
    /**
     * Instantiates a new  AlpacaAPI. Use this constructor if you are using the Trading API with OAuth to act on
     * behalf of an Alpaca account.
     *
     * @param traderOAuthToken      the Trader OAuth token
     * @param traderAPIEndpoint     the TraderAPIEndpoint
     */
    AlpacaAPI(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint);

    /**
     * Instantiates a new AlpacaAPI. Use this constructor if you are using the Broker API.
     *
     * @param brokerAPIKey          the Broker API key
     * @param brokerAPISecret       the Broker API secret
     * @param brokerAPIEndpoint     the BrokerAPIEndpoint
     */
    AlpacaAPI(const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint);

    /**
     * Instantiates a new AlpacaAPI.
     *
     * @param traderKeyID                   the Trader key ID
     * @param traderSecretKey               the Trader secret key
     * @param traderOAuthToken              the Trader OAuth token
     * @param traderAPIEndpoint             the TraderAPIEndpoint
     * @param marketDataWebsocketSource     the MarketDataWebsocketSource
     * @param brokerAPIKey                  the Broker API key
     * @param brokerAPISecret               the Broker API secret
     * @param brokerAPIEndpoint             the BrokerAPIEndpoint
     * 
     */
    AlpacaAPI(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint, const MarketDataWebsocketSource& marketDataWebsocketSource, const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint);

    AlpacaTraderAPI trader;

    /// REST Market Data API — stocks, options, crypto, forex, news, screener, etc.
    /// Available when constructed with trader API keys.
    AlpacaMarketDataAPI marketData;

    /// WebSocket stream for trade / order updates (binary frames).
    /// Available when constructed with trader API keys.
    std::unique_ptr<AlpacaUpdatesStream> updatesStream;

    /// WebSocket stream for real-time market data — trades, quotes, bars (text frames).
    /// Available when constructed with trader API keys and a market-data source.
    std::unique_ptr<AlpacaMarketDataStream> marketDataStream;

};

} // namespace alpaca