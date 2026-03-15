#pragma once

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/api/rest/HTTPClient.h"
#include "alpaca/model/trader/account.h"
#include "alpaca/model/trader/activity.h"
#include "alpaca/model/trader/asset.h"
#include "alpaca/model/trader/option.h"
#include "alpaca/model/trader/treasury.h"
#include "alpaca/model/trader/order.h"
#include "alpaca/model/trader/position.h"
#include "alpaca/model/trader/portfolio.h"
#include "alpaca/model/trader/watchlist.h"
#include "alpaca/model/trader/calendar.h"
#include "alpaca/model/trader/clock.h"
#include "alpaca/model/trader/wallet.h"

namespace alpaca {

using json = nlohmann::json;

class AlpacaTraderAPI {

    protected:

    std::string traderKeyID;
    std::string traderKeySecret;
    std::string traderOAuthToken;
    TraderAPIEndpoint traderAPIEndpoint;

    HttpClient httpClient;

    public:

    /**
     * Instantiates a new AlpacaTraderAPI.
     *
     * @param traderKeyID           the Trader key ID
     * @param traderSecretKey       the Trader secret key
     * @param traderOAuthToken      the Trader OAuth token
     * @param traderAPIEndpoint     the TraderAPIEndpoint
     * 
     */
    AlpacaTraderAPI(const std::string& traderKeyID, const std::string& traderKeySecret, const TraderAPIEndpoint& traderAPIEndpoint) 
    :
        traderKeyID(traderKeyID),
        traderKeySecret(traderKeySecret),
        traderAPIEndpoint(traderAPIEndpoint),
        httpClient(traderKeyID, traderKeySecret, traderAPIEndpoint)
    { 
    }

    /**
     * Instantiates a new AlpacaTraderAPI with OAuth token.
     */
    AlpacaTraderAPI(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint)
    :
        traderOAuthToken(traderOAuthToken),
        traderAPIEndpoint(traderAPIEndpoint),
        httpClient(traderOAuthToken, traderAPIEndpoint)
    { 
    }


    // ── Account ──────────────────────────────────────────────────────────
    Account get_account();
    AccountConfig get_account_config();
    AccountConfig send_account_config(const AccountConfig& account_config);

    // ── Assets ───────────────────────────────────────────────────────────
    std::vector<Asset> get_assets(const std::string& status = "", const std::string& asset_class = "", const std::string& exchange = "", const std::vector<std::string>& attributes = {});
    Asset get_asset(const std::string& symbol_id);

    // ── Options ──────────────────────────────────────────────────────────
    std::vector<OptionContract> get_option_contracts(const std::vector<std::string>& underlying_symbols = {}, bool show_deliverables = true, OptionContractStatus status = OptionContractStatus::ACTIVE, const DateTime& expiration_date = DateTime(), const DateTime& expiration_date_gte = DateTime(), const DateTime& expiration_date_lte = DateTime(), const std::string& root_symbol = "", OptionContractType type = OptionContractType::CALL, OptionContractStyle style = OptionContractStyle::AMERICAN, const Decimal& strike_price_gte = Decimal{}, const Decimal& strike_price_lte = Decimal{}, const std::string& page_token = "", int limit = 100, bool ppind = true);
    OptionContract get_option_contract(const std::string& symbol_id);
    void exercise_option(const std::string& symbol_contract_id);
    void do_not_exercise_option(const std::string& symbol_contract_id);

    // ── Treasuries ───────────────────────────────────────────────────────
    std::vector<Treasury> get_us_treasuries(const Subtype& subtype, const BondStatus& bond_status, const std::vector<std::string>& cusips, const std::vector<std::string>& isins);

    // ── Orders ───────────────────────────────────────────────────────────
    OrderResponse create_order(const Order& order);
    std::vector<OrderResponse> get_orders(OrderStatus status, int limit, const DateTime& after, const DateTime& until, Sort direction, bool nested, const std::vector<std::string>& symbols, OrderSide side);
    std::map<std::string, int> delete_all_orders();
    OrderResponse get_order_by_client_order_id(const std::string& client_order_id);
    OrderResponse get_order_by_id(const std::string& order_id, bool nested = false);
    OrderResponse replace_order(const std::string& order_id, int qty, OrderTimeInForce time_in_force, const Decimal& limit_price, const Decimal& stop_price, const Decimal& trail, const std::string& client_order_id);
    void delete_order(const std::string& order_id);

    // ── Positions ────────────────────────────────────────────────────────
    std::vector<Position> get_all_positions();
    std::vector<PositionClosed> close_all_positions(bool cancel_orders = true);
    Position get_open_position(const std::string& symbol_id);
    OrderResponse close_position(const std::string& symbol_id, const Decimal& qty = Decimal{}, const Decimal& percentage = Decimal{});

    // ── Portfolio ────────────────────────────────────────────────────────
    PortfolioHistory get_portfolio_history(const std::string& period, const std::string& timeframe, IntradayReporting reporting, const DateTime& start, PnlReset reset, const DateTime& end, const std::string& cashflow_types);

    // ── Watchlists ───────────────────────────────────────────────────────
    std::vector<Watchlist> get_all_watchlists();
    Watchlist create_watchlist(const std::string& name, const std::vector<std::string>& symbols);
    Watchlist get_watchlist_by_id(const std::string& id);
    Watchlist update_watchlist(const std::string& id, const std::string& name, const std::vector<std::string>& symbols);
    Watchlist add_asset_to_watchlist(const std::string& id, const std::string& symbol);
    void delete_watchlist(const std::string& id);
    Watchlist get_watchlist_by_name(const std::string& name);
    Watchlist update_watchlist_by_name(const std::string& name, const std::vector<std::string>& symbols);
    Watchlist add_asset_to_watchlist_by_name(const std::string& name, const std::string& symbol);
    void delete_watchlist_by_name(const std::string& name);
    Watchlist remove_asset_from_watchlist(const std::string& id, const std::string& symbol);

    // ── Activities ───────────────────────────────────────────────────────
    std::vector<AccountActivity> get_account_activities(const DateTime& after = DateTime(), const DateTime& until = DateTime(), Sort direction = Sort::DESC, int page_size = 100, const std::string& page_token = "", const std::string& category = "");
    std::vector<AccountActivity> get_account_activities_by_type(const std::string& activity_type, const DateTime& after = DateTime(), const DateTime& until = DateTime(), Sort direction = Sort::DESC, int page_size = 100, const std::string& page_token = "");

    // ── Calendar & Clock ─────────────────────────────────────────────────
    Calendar get_calendar(const DateTime& start = DateTime(), const DateTime& end = DateTime());
    Clock get_clock();
    Calendar get_calendar_v3(const std::string& market, const DateTime& start = DateTime(), const DateTime& end = DateTime());
    Clock get_clock_v3();

    // ── Crypto Wallets ───────────────────────────────────────────────────
    std::vector<CryptoWallet> get_crypto_wallets();
    std::vector<CryptoTransfer> get_crypto_transfers(const std::string& asset = "");
    CryptoTransfer create_crypto_transfer(const CryptoTransferRequest& request);
    CryptoTransfer get_crypto_transfer(const std::string& transfer_id);
    std::vector<WhitelistedAddress> get_whitelisted_addresses();
    WhitelistedAddress create_whitelisted_address(const WhitelistedAddressRequest& request);
    void delete_whitelisted_address(const std::string& whitelisted_address_id);
    CryptoTransferEstimate get_crypto_transfer_estimate(const std::string& asset, const std::string& network = "", const Decimal& amount = Decimal{});

};


} // namespace alpaca