#pragma once

#include <iostream>
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

namespace alpaca {

using json = nlohmann::json;

class AlpacaTraderAPI {

    private:

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
     * 
     */
    AlpacaTraderAPI(const std::string& traderOAuthToken, const TraderAPIEndpoint& traderAPIEndpoint)
    :
        traderOAuthToken(traderOAuthToken),
        traderAPIEndpoint(traderAPIEndpoint),
        httpClient(traderOAuthToken, traderAPIEndpoint)
    { 
    }


    /**
     * 
     */
    Account get_account();

    /**
     * 
     */
    AccountConfig send_account_config(const AccountConfig& account_config);

    /**
     * 
     */
    std::vector<Asset> get_assets(const std::string& status = "", const std::string& asset_class = "", const std::string& exchange = "", const std::vector<std::string>& attributes = {});

    /**
     * 
     */
    Asset get_asset(const std::string& symbol_id);

    /**
     * 
     */
    std::vector<OptionContract> get_option_contracts(const std::vector<std::string>& underlying_symbols = {}, const bool& show_deliverables = true, const OptionContractStatus& status = OptionContractStatus::ACTIVE, const DateTime& expiration_date = DateTime(), const DateTime& expiration_date_gte = DateTime(), const DateTime& expiration_date_lte = DateTime(), const std::string& root_symbol = "", const OptionContractType& type = OptionContractType::CALL, const OptionContractStyle& style = OptionContractStyle::AMERICAN, const double& strike_price_gte = 0.0, const double& strike_price_lte = 0.0, const std::string& page_token = "", const int& limit = 100, const bool& ppind = true);

    /**
     * 
     */
    OptionContract get_option_contract(const std::string& symbol_id);

    /**
     * 
     */
    std::vector<Treasury> get_us_treasuries(const Subtype& subtype, const BondStatus& bond_status, const std::vector<std::string>& cusips, const std::vector<std::string>& isins);

    /**
     * 
     */
    OrderResponse create_order(const Order& order);

    /**
     * 
     */
    std::vector<OrderResponse> get_orders(const OrderStatus& status, const int& limit, const DateTime& after, const DateTime& until, const Sort& direction, const bool& nested, const std::vector<std::string>& symbols, const OrderSide& side);

    /**
     * 
     */
    std::map<std::string, int> delete_all_orders();

    /**
     * 
     */
    OrderResponse get_order_by_client_order_id(const std::string& client_order_id);

    /**
     * 
     */
    OrderResponse get_order_by_id(const std::string& order_id, const bool& nested = false);

    /**
     * 
     * "You can only patch full shares for now." - Alpaca
     */
    OrderResponse replace_order(const std::string& order_id, const int& qty, const OrderTimeInForce& time_in_force, const double& limit_price, const double& stop_price, const double& trail, std::string& client_order_id);

    /**
     * 
     */
    void delete_order(const std::string& order_id);

    /**
     * 
     */
    std::vector<Position> get_all_positions();

    /**
     * 
     */
    std::vector<PositionClosed> close_all_positions(const bool& cancel_orders = true);

    /**
     * 
     */
    Position get_open_position(const std::string& symbol_id);

    /**
     * 
     */
    Order close_position(const std::string& symbol_id, const double& qty = 0.0, const double& percentage = 0.0);

    /**
     * 
     */
    void exercise_option(const std::string& symbol_contract_id);

    /**
     * 
     */
    PortfolioHistory get_portfolio_history(const std::string& period, const std::string& timeframe, const IntradayReporting& reporting, const DateTime& start, const PnlReset& reset, const DateTime& end, const std::string& cashflow_types);

    /**
     * 
     */
    std::vector<Watchlist> get_all_watchlists();

    /**
     * 
     */
    Watchlist create_watchlist(const std::string& name, const std::vector<std::string>& symbols);

    /**
     * 
     */
    Watchlist get_watchlist_by_id(const std::string& id);

    /**
     * 
     */
    

};




} // namespace alpaca