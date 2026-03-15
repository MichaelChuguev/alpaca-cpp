#include "alpaca/AlpacaAPI.h"
#include "alpaca/api/rest/HTTPClient.h"

namespace alpaca {

    AlpacaAPI::AlpacaAPI(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint,
        const MarketDataWebsocketSource& marketDataWebsocketSource)
    : 
        traderKeyID(traderKeyID),
        traderKeySecret(traderKeySecret),
        traderOAuthToken(""), // Empty for API key auth
        traderAPIEndpoint(traderAPIEndpoint),
        marketDataWebsocketSource(marketDataWebsocketSource),
        marketDataEndpoint(traderAPIEndpoint == TraderAPIEndpoint::LIVE ? MarketDataEndpoint::LIVE : MarketDataEndpoint::SANDBOX),
        brokerAPIKey(""), // Not used in this constructor
        brokerAPISecret(""), // Not used in this constructor
        brokerAPIEndpoint(BrokerAPIEndpoint::SANDBOX), // Default value (unused)
        trader(traderKeyID, traderKeySecret, traderAPIEndpoint),
        marketData(traderKeyID, traderKeySecret,
            traderAPIEndpoint == TraderAPIEndpoint::LIVE ? MarketDataEndpoint::LIVE : MarketDataEndpoint::SANDBOX,
            marketDataWebsocketSource == MarketDataWebsocketSource::SIP ? DataFeed::SIP : DataFeed::IEX),
        updatesStream(std::make_unique<AlpacaUpdatesStream>(traderKeyID, traderKeySecret, traderAPIEndpoint)),
        marketDataStream(std::make_unique<AlpacaMarketDataStream>(traderKeyID, traderKeySecret,
            marketDataWebsocketSource == MarketDataWebsocketSource::SIP ? MarketDataFeed::SIP_FEED : MarketDataFeed::IEX_FEED)),
        broker("", "", BrokerAPIEndpoint::SANDBOX)
    {
    }

    AlpacaAPI::AlpacaAPI(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint)
    : 
        traderKeyID(""), // Empty for OAuth token auth
        traderKeySecret(""), // Empty for OAuth token auth
        traderOAuthToken(traderOAuthToken),
        traderAPIEndpoint(traderAPIEndpoint),
        marketDataWebsocketSource(MarketDataWebsocketSource::IEX), // Default value (unused)
        marketDataEndpoint(traderAPIEndpoint == TraderAPIEndpoint::LIVE ? MarketDataEndpoint::LIVE : MarketDataEndpoint::SANDBOX),
        brokerAPIKey(""), // Not used in this constructor
        brokerAPISecret(""), // Not used in this constructor
        brokerAPIEndpoint(BrokerAPIEndpoint::SANDBOX), // Default value (unused)
        trader(traderOAuthToken, traderAPIEndpoint),
        marketData("", "",
            traderAPIEndpoint == TraderAPIEndpoint::LIVE ? MarketDataEndpoint::LIVE : MarketDataEndpoint::SANDBOX,
            DataFeed::IEX),
        broker("", "", BrokerAPIEndpoint::SANDBOX)
    {
    }

    AlpacaAPI::AlpacaAPI(const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint)
    :
        traderKeyID(""),
        traderKeySecret(""),
        traderOAuthToken(""),
        traderAPIEndpoint(TraderAPIEndpoint::PAPER),
        marketDataWebsocketSource(MarketDataWebsocketSource::IEX),
        marketDataEndpoint(MarketDataEndpoint::SANDBOX),
        brokerAPIKey(brokerAPIKey),
        brokerAPISecret(brokerAPISecret),
        brokerAPIEndpoint(brokerAPIEndpoint),
        trader("", "", TraderAPIEndpoint::PAPER),
        marketData("", "", MarketDataEndpoint::SANDBOX, DataFeed::IEX),
        broker(brokerAPIKey, brokerAPISecret, brokerAPIEndpoint)
    {
    }

    AlpacaAPI::AlpacaAPI(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint,
        const MarketDataWebsocketSource& marketDataWebsocketSource, const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint)
    :
        traderKeyID(traderKeyID),
        traderKeySecret(traderKeySecret),
        traderOAuthToken(""),
        traderAPIEndpoint(traderAPIEndpoint),
        marketDataWebsocketSource(marketDataWebsocketSource),
        marketDataEndpoint(traderAPIEndpoint == TraderAPIEndpoint::LIVE ? MarketDataEndpoint::LIVE : MarketDataEndpoint::SANDBOX),
        brokerAPIKey(brokerAPIKey),
        brokerAPISecret(brokerAPISecret),
        brokerAPIEndpoint(brokerAPIEndpoint),
        trader(traderKeyID, traderKeySecret, traderAPIEndpoint),
        marketData(traderKeyID, traderKeySecret,
            traderAPIEndpoint == TraderAPIEndpoint::LIVE ? MarketDataEndpoint::LIVE : MarketDataEndpoint::SANDBOX,
            marketDataWebsocketSource == MarketDataWebsocketSource::SIP ? DataFeed::SIP : DataFeed::IEX),
        updatesStream(std::make_unique<AlpacaUpdatesStream>(traderKeyID, traderKeySecret, traderAPIEndpoint)),
        marketDataStream(std::make_unique<AlpacaMarketDataStream>(traderKeyID, traderKeySecret,
            marketDataWebsocketSource == MarketDataWebsocketSource::SIP ? MarketDataFeed::SIP_FEED : MarketDataFeed::IEX_FEED)),
        broker(brokerAPIKey, brokerAPISecret, brokerAPIEndpoint)
    {
    }

} // namespace alpaca