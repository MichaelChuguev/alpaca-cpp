#pragma once

#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/api/rest/HTTPClient.h"
#include "alpaca/model/data/stock.h"
#include "alpaca/model/data/option_data.h"
#include "alpaca/model/data/crypto.h"
#include "alpaca/model/data/news.h"

namespace alpaca {

using json = nlohmann::json;

class AlpacaMarketDataAPI {

protected:

    std::string keyID;
    std::string keySecret;
    MarketDataEndpoint marketDataEndpoint;
    DataFeed defaultDataFeed;
    OptionFeed defaultOptionFeed;

    HttpClient httpClient;

    /** Wrapper for HTTP GET to allow focused API tests to stub responses. */
    virtual json get_json(const std::string& endpoint) {
        return httpClient.get(endpoint);
    }

    /** Resolve DataFeed::DEFAULT to the stored default. */
    DataFeed resolve_feed(DataFeed feed) const {
        return feed == DataFeed::DEFAULT ? defaultDataFeed : feed;
    }

    /** Resolve OptionFeed::DEFAULT to the stored default. */
    OptionFeed resolve_option_feed(OptionFeed feed) const {
        return feed == OptionFeed::DEFAULT ? defaultOptionFeed : feed;
    }

public:

    /**
     * Instantiates a new AlpacaMarketDataAPI.
     *
     * @param keyID                 the API key ID
     * @param keySecret             the API secret key
     * @param marketDataEndpoint    the MarketDataEndpoint (SANDBOX or LIVE)
     * @param defaultDataFeed       the default DataFeed for stock endpoints (IEX, SIP, etc.)
     * @param defaultOptionFeed     the default OptionFeed for option snapshot/latest endpoints
     */
    AlpacaMarketDataAPI(const std::string& keyID, const std::string& keySecret,
                        const MarketDataEndpoint& marketDataEndpoint,
                        DataFeed defaultDataFeed = DataFeed::IEX,
                        OptionFeed defaultOptionFeed = OptionFeed::DEFAULT)
        : keyID(keyID),
          keySecret(keySecret),
          marketDataEndpoint(marketDataEndpoint),
          defaultDataFeed(defaultDataFeed),
          defaultOptionFeed(defaultOptionFeed),
          httpClient(keyID, keySecret, marketDataEndpoint)
    {}

    virtual ~AlpacaMarketDataAPI() = default;

    // =====================================================================
    //  STOCK ENDPOINTS (/v2/stocks/...)
    // =====================================================================

    // ── Trades ───────────────────────────────────────────────────────────

    /** Multi-symbol historical trades. */
    std::map<std::string, std::vector<StockTrade>> get_stock_trades(
        const std::vector<std::string>& symbols,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Single-symbol historical trades. */
    std::vector<StockTrade> get_stock_trades_single(
        const std::string& symbol,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Multi-symbol latest trades. */
    std::map<std::string, StockTrade> get_stock_latest_trades(
        const std::vector<std::string>& symbols,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    /** Single-symbol latest trade. */
    StockTrade get_stock_latest_trade_single(
        const std::string& symbol,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    // ── Quotes ───────────────────────────────────────────────────────────

    /** Multi-symbol historical quotes. */
    std::map<std::string, std::vector<StockQuote>> get_stock_quotes(
        const std::vector<std::string>& symbols,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Single-symbol historical quotes. */
    std::vector<StockQuote> get_stock_quotes_single(
        const std::string& symbol,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Multi-symbol latest quotes. */
    std::map<std::string, StockQuote> get_stock_latest_quotes(
        const std::vector<std::string>& symbols,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    /** Single-symbol latest quote. */
    StockQuote get_stock_latest_quote_single(
        const std::string& symbol,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    // ── Bars ─────────────────────────────────────────────────────────────

    /** Multi-symbol historical bars. */
    std::map<std::string, std::vector<StockBar>> get_stock_bars(
        const std::vector<std::string>& symbols,
        DataTimeframe timeframe = DataTimeframe::ONE_DAY,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC,
        const std::string& adjustment = "");

    /** Single-symbol historical bars. */
    std::vector<StockBar> get_stock_bars_single(
        const std::string& symbol,
        DataTimeframe timeframe = DataTimeframe::ONE_DAY,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC,
        const std::string& adjustment = "");

    /** Multi-symbol latest bars. */
    std::map<std::string, StockBar> get_stock_latest_bars(
        const std::vector<std::string>& symbols,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    /** Single-symbol latest bar. */
    StockBar get_stock_latest_bar_single(
        const std::string& symbol,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    // ── Auctions ─────────────────────────────────────────────────────────

    /** Multi-symbol auctions. */
    std::map<std::string, std::vector<StockAuction>> get_stock_auctions(
        const std::vector<std::string>& symbols,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Single-symbol auctions. */
    std::vector<StockAuction> get_stock_auctions_single(
        const std::string& symbol,
        const std::string& start = "", const std::string& end = "",
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    // ── Snapshots ────────────────────────────────────────────────────────

    /** Multi-symbol snapshots. */
    std::map<std::string, StockSnapshot> get_stock_snapshots(
        const std::vector<std::string>& symbols,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    /** Single-symbol snapshot. */
    StockSnapshot get_stock_snapshot_single(
        const std::string& symbol,
        DataFeed feed = DataFeed::DEFAULT,
        const std::string& currency = "");

    // ── Meta ─────────────────────────────────────────────────────────────

    /** Stock meta conditions for a given tick type. */
    std::map<std::string, std::string> get_stock_meta_conditions(TickType tick_type);

    /** Stock meta exchanges. */
    std::map<std::string, std::string> get_stock_meta_exchanges();

    // =====================================================================
    //  OPTION ENDPOINTS (/v1beta1/options/...)
    // =====================================================================

    /** Option bars (historical). */
    std::map<std::string, std::vector<OptionBar>> get_option_bars(
        const std::vector<std::string>& symbols,
        DataTimeframe timeframe = DataTimeframe::ONE_DAY,
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Option trades (historical). */
    std::map<std::string, std::vector<OptionTrade>> get_option_trades(
        const std::vector<std::string>& symbols,
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Option latest trades (multi-symbol). Supports option feed selection. */
    std::map<std::string, OptionTrade> get_option_latest_trades(
        const std::vector<std::string>& symbols,
        OptionFeed feed = OptionFeed::DEFAULT);

    /** Option latest quotes (multi-symbol). Supports option feed selection. */
    std::map<std::string, OptionQuote> get_option_latest_quotes(
        const std::vector<std::string>& symbols,
        OptionFeed feed = OptionFeed::DEFAULT);

    /**
     * Option snapshots (multi-symbol). Supports option feed selection.
     *
     */
    std::map<std::string, OptionSnapshot> get_option_snapshots(
        const std::vector<std::string>& symbols,
        OptionFeed feed = OptionFeed::DEFAULT,
        int limit = 0, const std::string& page_token = "");

    /**
     * Option chain (by underlying symbol).
     *
     * Map of <symbol, OptionSnapshot>.
     * OptionSnapshot also contains the contract symbol, so for the full snapshot alone just use option_chain.second 
     */
    std::map<std::string, OptionSnapshot> get_option_chain(
        const std::string& underlying_symbol,
        OptionFeed feed = OptionFeed::DEFAULT,
        const std::string& page_token = "",
        int limit = 0);

    /** Option meta conditions for a given tick type. */
    std::map<std::string, std::string> get_option_meta_conditions(TickType tick_type);

    /** Option meta exchanges. */
    std::map<std::string, std::string> get_option_meta_exchanges();

    // =====================================================================
    //  CRYPTO ENDPOINTS (/v1beta3/crypto/{loc}/...)
    // =====================================================================

    /** Crypto bars (historical). */
    std::map<std::string, std::vector<CryptoBar>> get_crypto_bars(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US,
        DataTimeframe timeframe = DataTimeframe::ONE_DAY,
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Crypto latest bars (multi-symbol). */
    std::map<std::string, CryptoBar> get_crypto_latest_bars(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US);

    /** Crypto trades (historical). */
    std::map<std::string, std::vector<CryptoTrade>> get_crypto_trades(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US,
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Crypto latest trades (multi-symbol). */
    std::map<std::string, CryptoTrade> get_crypto_latest_trades(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US);

    /** Crypto quotes (historical). */
    std::map<std::string, std::vector<CryptoQuote>> get_crypto_quotes(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US,
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    /** Crypto latest quotes (multi-symbol). */
    std::map<std::string, CryptoQuote> get_crypto_latest_quotes(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US);

    /** Crypto latest orderbooks (multi-symbol). */
    std::map<std::string, CryptoOrderbook> get_crypto_latest_orderbooks(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US);

    /** Crypto snapshots (multi-symbol). */
    std::map<std::string, CryptoSnapshot> get_crypto_snapshots(
        const std::vector<std::string>& symbols,
        CryptoLoc loc = CryptoLoc::US);

    // =====================================================================
    //  FOREX ENDPOINTS (/v1beta1/forex/...)
    // =====================================================================

    /** Latest forex rates for given currency pairs. */
    std::map<std::string, ForexRate> get_forex_latest_rates(
        const std::vector<std::string>& currency_pairs);

    /** Historical forex rates. */
    std::map<std::string, std::vector<ForexRate>> get_forex_rates(
        const std::vector<std::string>& currency_pairs,
        DataTimeframe timeframe = DataTimeframe::ONE_DAY,
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

    // =====================================================================
    //  FIXED INCOME (/v1beta1/fixed_income/...)
    // =====================================================================

    /** Latest fixed income prices. */
    std::map<std::string, FixedIncomePrice> get_fixed_income_latest_prices(
        const std::vector<std::string>& symbols);

    // =====================================================================
    //  LOGOS (/v1beta1/logos/...)
    // =====================================================================

    /** Get logo URL for a symbol (returns the raw URL string). */
    std::string get_logo_url(const std::string& symbol);

    // =====================================================================
    //  SCREENER (/v1beta1/screener/...)
    // =====================================================================

    /** Screener: most active stocks. */
    std::vector<MostActive> get_most_actives(int top = 20);

    /** Screener: top movers for a given market type. */
    std::vector<Mover> get_movers(ScreenerMarketType market_type, int top = 20);

    // =====================================================================
    //  NEWS (/v1beta1/news)
    // =====================================================================

    /** Get news articles. */
    std::vector<NewsArticle> get_news(
        const std::vector<std::string>& symbols = {},
        const std::string& start = "", const std::string& end = "",
        int limit = 10, const std::string& page_token = "",
        Sort sort = Sort::DESC,
        bool include_content = false);

    // =====================================================================
    //  CORPORATE ACTIONS (/v1/corporate-actions)
    // =====================================================================

    /** Get corporate actions. */
    json get_corporate_actions(
        const std::vector<std::string>& symbols = {},
        const std::vector<std::string>& types = {},
        const std::string& start = "", const std::string& end = "",
        int limit = 0, const std::string& page_token = "",
        Sort sort = Sort::ASC);

};

} // namespace alpaca
