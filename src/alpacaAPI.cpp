#include "alpaca/alpacaAPI.h"
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
        brokerAPIKey(""), // Not used in this constructor
        brokerAPISecret(""), // Not used in this constructor
        brokerAPIEndpoint(SANDBOX), // Default value (unused)
        trader(traderKeyID, traderKeySecret, traderAPIEndpoint)
    {
    }

    AlpacaAPI::AlpacaAPI(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint)
    : 
        traderKeyID(""), // Empty for OAuth token auth
        traderKeySecret(""), // Empty for OAuth token auth
        traderOAuthToken(traderOAuthToken),
        traderAPIEndpoint(traderAPIEndpoint),
        marketDataWebsocketSource(IEX), // Default value (unused)
        brokerAPIKey(""), // Not used in this constructor
        brokerAPISecret(""), // Not used in this constructor
        brokerAPIEndpoint(SANDBOX), // Default value (unused)
        trader(traderOAuthToken, traderAPIEndpoint)
    {
    }












} // namespace alpaca