#pragma once

#include <string>
#include <vector>
#include <map>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/api/rest/HTTPClient.h"
#include "alpaca/BrokerSSEClient.h"
#include "alpaca/model/broker/common.h"
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

class AlpacaBrokerAPI {

protected:
    std::string brokerAPIKey;
    std::string brokerAPISecret;
    BrokerAPIEndpoint brokerAPIEndpoint;
    BrokerAuthMode brokerAuthMode;

    HttpClient httpClient;

    std::string broker_host() const;
    std::string make_sse_url(const std::string& path) const;

public:
    // Legacy auth: authenticate with APCA-API-KEY-ID / APCA-API-SECRET-KEY headers.
    AlpacaBrokerAPI(const std::string& brokerAPIKey, const std::string& brokerAPISecret, const BrokerAPIEndpoint& brokerAPIEndpoint)
    :
        brokerAPIKey(brokerAPIKey),
        brokerAPISecret(brokerAPISecret),
        brokerAPIEndpoint(brokerAPIEndpoint),
        brokerAuthMode(BrokerAuthMode::LEGACY),
        httpClient(brokerAPIKey, brokerAPISecret, brokerAPIEndpoint)
    {
    }

    // Client-credentials auth: exchange clientId + clientSecret for a short-lived
    // OAuth2 Bearer token at authx.alpaca.markets.  The token is cached and
    // refreshed automatically before it expires (15-minute lifetime).
    AlpacaBrokerAPI(const std::string& clientId, const std::string& clientSecret, const BrokerAPIEndpoint& brokerAPIEndpoint, BrokerAuthMode authMode)
    :
        brokerAPIKey(clientId),
        brokerAPISecret(clientSecret),
        brokerAPIEndpoint(brokerAPIEndpoint),
        brokerAuthMode(authMode),
        httpClient(clientId, clientSecret, brokerAPIEndpoint, authMode)
    {
    }

    // ── Accounts ─────────────────────────────────────────────────────────
    std::vector<BrokerEntity> get_accounts(const std::string& query = "");
    BrokerEntity create_account(const json& account_request);
    BrokerEntity get_account(const std::string& account_id);
    BrokerEntity patch_account(const std::string& account_id, const json& patch_request);
    BrokerEntity request_options_for_account(const std::string& account_id, const json& request);
    std::vector<BrokerEntity> request_list_options_approvals(const std::string& query = "");
    std::vector<BrokerEntity> get_account_activities(const std::string& query = "");
    std::vector<BrokerEntity> get_account_activities_by_type(const std::string& activity_type, const std::string& query = "");
    Account get_trading_account(const std::string& account_id);
    BrokerEntity get_pdt_status(const std::string& account_id);
    BrokerEntity pdt_one_time_removal(const std::string& account_id);
    BrokerEntity close_account(const std::string& account_id, const json& request = json::object());

    // ── Account Documents ────────────────────────────────────────────────
    std::vector<BrokerEntity> get_docs_for_account(const std::string& account_id);
    BrokerEntity upload_doc_to_account(const std::string& account_id, const json& request);
    BrokerEntity download_doc_from_account(const std::string& account_id, const std::string& document_id);
    BrokerEntity download_w8ben_doc_from_account(const std::string& account_id, const std::string& document_id);

    // ── Trading: Positions ───────────────────────────────────────────────
    std::vector<Position> get_positions_for_account(const std::string& account_id);
    std::vector<PositionClosed> close_all_positions_for_account(const std::string& account_id, bool cancel_orders = true);
    Position get_position_for_account_by_symbol(const std::string& account_id, const std::string& symbol_or_asset_id);
    OrderResponse close_position_for_account_by_symbol(const std::string& account_id, const std::string& symbol_or_asset_id, const Decimal& qty = Decimal{}, const Decimal& percentage = Decimal{});
    void option_exercise_for_account(const std::string& account_id, const std::string& symbol_or_contract_id);
    void option_do_not_exercise_for_account(const std::string& account_id, const std::string& symbol_or_contract_id);

    // ── Trading: Orders ──────────────────────────────────────────────────
    OrderResponse get_order_for_account(const std::string& account_id, const std::string& order_id, bool nested = false);
    OrderResponse replace_order_for_account(const std::string& account_id, const std::string& order_id, const json& patch_request);
    void delete_order_for_account(const std::string& account_id, const std::string& order_id);
    std::vector<OrderResponse> get_all_orders_for_account(const std::string& account_id, OrderStatus status = OrderStatus::OPEN, int limit = 100, const DateTime& after = DateTime(), const DateTime& until = DateTime(), Sort direction = Sort::DESC, bool nested = false, const std::vector<std::string>& symbols = {});
    OrderResponse get_order_by_client_order_id_for_account(const std::string& account_id, const std::string& client_order_id);
    OrderResponse create_order_for_account(const std::string& account_id, const Order& order);
    std::map<std::string, int> delete_all_orders_for_account(const std::string& account_id);
    BrokerEntity get_order_estimation_for_account(const std::string& account_id, const json& order_request);

    // ── Trading: Account Config / Limits / Portfolio ────────────────────
    AccountConfig patch_trading_account_configurations(const std::string& account_id, const AccountConfig& config);
    BrokerEntity get_account_trading_limits(const std::string& account_id);
    PortfolioHistory get_portfolio_history_for_account(const std::string& account_id, const std::string& period, const std::string& timeframe, IntradayReporting reporting, const DateTime& start, PnlReset reset, const DateTime& end, const std::string& cashflow_types);

    // ── Instant Funding ──────────────────────────────────────────────────
    std::vector<BrokerEntity> get_instant_funding_list(const std::string& query = "");
    BrokerEntity create_instant_funding(const json& request);
    BrokerEntity get_instant_funding_single(const std::string& instant_funding_id);
    void delete_instant_funding_single(const std::string& instant_funding_id);
    std::vector<BrokerEntity> get_instant_funding_settlements(const std::string& query = "");
    BrokerEntity create_instant_funding_settlement(const json& request);
    BrokerEntity get_instant_funding_settlement_single(const std::string& settlement_id);
    std::vector<BrokerEntity> get_instant_funding_correspondent_limits(const std::string& query = "");
    std::vector<BrokerEntity> get_instant_funding_account_limits(const std::string& query = "");
    std::vector<BrokerEntity> get_instant_funding_reports(const std::string& query = "");

    // ── Broker Assets / Options / Treasuries / Clock / Calendar ─────────
    std::vector<Asset> get_assets(const std::string& status = "", const std::string& asset_class = "", const std::string& exchange = "", const std::vector<std::string>& attributes = {});
    Asset get_asset(const std::string& symbol_or_asset_id);
    std::vector<BrokerEntity> get_asset_entry_requirements(const std::string& query = "");
    std::vector<OptionContract> get_option_contracts(const std::vector<std::string>& underlying_symbols = {}, bool show_deliverables = true, OptionContractStatus status = OptionContractStatus::ACTIVE, const DateTime& expiration_date = DateTime(), const DateTime& expiration_date_gte = DateTime(), const DateTime& expiration_date_lte = DateTime(), const std::string& root_symbol = "", OptionContractType type = OptionContractType::CALL, OptionContractStyle style = OptionContractStyle::AMERICAN, const Decimal& strike_price_gte = Decimal{}, const Decimal& strike_price_lte = Decimal{}, const std::string& page_token = "", int limit = 100, bool ppind = true);
    OptionContract get_option_contract(const std::string& symbol_or_id);
    std::vector<Treasury> get_us_treasuries(const Subtype& subtype, const BondStatus& bond_status, const std::vector<std::string>& cusips, const std::vector<std::string>& isins);
    Calendar get_calendar(const DateTime& start = DateTime(), const DateTime& end = DateTime());
    Calendar get_calendar_v2(const std::string& market, const DateTime& start = DateTime(), const DateTime& end = DateTime());
    Clock get_clock();
    Clock get_clock_v2();

    // ── Events (SSE) ─────────────────────────────────────────────────────
    BrokerSSESubscription subscribe_to_account_status_sse() const;
    BrokerSSESubscription subscribe_to_journal_status_sse() const;
    BrokerSSESubscription subscribe_to_journal_status_v2_sse() const;
    BrokerSSESubscription subscribe_to_trade_v2_sse() const;
    BrokerSSESubscription subscribe_to_admin_action_sse() const;
    std::vector<BrokerEntity> get_v1_events_nta(const std::string& query = "");
    BrokerSSESubscription subscribe_to_funding_status_sse() const;
    BrokerSSESubscription subscribe_to_system_event_v2_sse() const;

    // Convenience wrappers that return a ready-to-run SSE client bound to
    // this AlpacaBrokerAPI instance credentials.
    BrokerSSEClient create_sse_client(const BrokerSSESubscription& subscription) const;
    BrokerSSEClient create_account_status_sse_client() const;
    BrokerSSEClient create_journal_status_sse_client() const;
    BrokerSSEClient create_journal_status_v2_sse_client() const;
    BrokerSSEClient create_trade_v2_sse_client() const;
    BrokerSSEClient create_admin_action_sse_client() const;
    BrokerSSEClient create_funding_status_sse_client() const;
    BrokerSSEClient create_system_event_v2_sse_client() const;

    // ── Funding: Recipient Banks / Transfers / ACH / JIT ────────────────
    std::vector<BrokerEntity> get_recipient_banks(const std::string& account_id);
    BrokerEntity create_recipient_bank(const std::string& account_id, const json& request);
    void delete_recipient_bank(const std::string& account_id, const std::string& bank_id);
    std::vector<BrokerEntity> get_transfers_for_account(const std::string& account_id, const std::string& query = "");
    BrokerEntity create_transfer_for_account(const std::string& account_id, const json& request);
    void delete_transfer(const std::string& account_id, const std::string& transfer_id);
    std::vector<BrokerEntity> get_account_ach_relationships(const std::string& account_id, const std::string& query = "");
    BrokerEntity create_ach_relationship_for_account(const std::string& account_id, const json& request);
    void delete_ach_relationship_from_account(const std::string& account_id, const std::string& ach_relationship_id);
    std::vector<BrokerEntity> get_v1_transfers_jit_reports(const std::string& query = "");
    std::vector<BrokerEntity> get_v1_transfers_jit_limits(const std::string& query = "");
    std::vector<BrokerEntity> get_v1_transfers_jit_ledgers(const std::string& query = "");
    std::vector<BrokerEntity> get_v1_transfers_jit_ledger_balances(const std::string& ledger_id, const std::string& query = "");
    std::vector<BrokerEntity> get_v1_jit_settlements(const std::string& query = "");
    BrokerEntity post_v1_jit_settlements(const json& request);
    BrokerEntity get_v1_jit_settlement_single(const std::string& settlement_id);

    // ── Cash Interest / OAuth / Journals / FPSL ─────────────────────────
    std::vector<BrokerEntity> get_v1_list_apr_tiers(const std::string& query = "");
    BrokerEntity get_oauth_client(const std::string& client_id);
    BrokerEntity issue_oauth_token(const std::vector<std::pair<std::string, std::string>>& form_fields);
    BrokerEntity authorize_oauth_token(const std::string& query = "");
    std::vector<BrokerEntity> get_all_journals(const std::string& query = "");
    BrokerEntity create_journal(const json& request);
    void delete_journal_by_id(const std::string& journal_id);
    BrokerEntity get_v1_journal(const std::string& journal_id);
    std::vector<BrokerEntity> create_batch_journal(const json& request);
    BrokerEntity reverse_batch_journal(const json& request);
    std::vector<BrokerEntity> get_v1_list_fpsl_tiers(const std::string& query = "");
    std::vector<BrokerEntity> get_v1_list_fpsl_loans(const std::string& query = "");

    // ── Trading Watchlists ───────────────────────────────────────────────
    std::vector<Watchlist> get_all_watchlists_for_account(const std::string& account_id);
    Watchlist create_watchlist_for_account(const std::string& account_id, const std::string& name, const std::vector<std::string>& symbols);
    Watchlist get_watchlist_for_account_by_id(const std::string& account_id, const std::string& watchlist_id);
    Watchlist replace_watchlist_for_account_by_id(const std::string& account_id, const std::string& watchlist_id, const std::string& name, const std::vector<std::string>& symbols);
    void delete_watchlist_from_account_by_id(const std::string& account_id, const std::string& watchlist_id);
    Watchlist add_asset_to_watchlist_for_account(const std::string& account_id, const std::string& watchlist_id, const std::string& symbol);
    Watchlist remove_asset_from_watchlist_for_account(const std::string& account_id, const std::string& watchlist_id, const std::string& symbol);

    // ── KYC / Identity ───────────────────────────────────────────────────
    BrokerEntity post_v1_accounts_cip(const std::string& account_id, const json& request);
    BrokerEntity get_v1_accounts_cip(const std::string& account_id);
    BrokerEntity get_v1_accounts_onfido_sdk_tokens(const std::string& account_id, const std::string& query = "");
    BrokerEntity patch_v1_accounts_onfido_sdk(const std::string& account_id, const json& request);

    // ── Banking Demo / Funding Wallet ────────────────────────────────────
    BrokerEntity demo_deposit_funding(const json& request);
    std::vector<BrokerEntity> batch_create_funding_wallets(const json& request);
    BrokerEntity get_funding_wallet(const std::string& account_id);
    BrokerEntity create_funding_wallet(const std::string& account_id, const json& request = json::object());
    std::vector<BrokerEntity> list_funding_details(const std::string& account_id, const std::string& query = "");
    std::vector<BrokerEntity> get_funding_wallet_transfers(const std::string& account_id, const std::string& query = "");
    BrokerEntity get_funding_wallet_transfer_by_id(const std::string& account_id, const std::string& transfer_id);
    std::vector<BrokerEntity> get_funding_wallet_recipient_bank(const std::string& account_id);
    BrokerEntity create_funding_wallet_recipient_bank(const std::string& account_id, const json& request);
    void delete_funding_wallet_recipient_bank(const std::string& account_id);
    BrokerEntity create_funding_wallet_withdrawal(const std::string& account_id, const json& request);

    // ── Rebalancing ──────────────────────────────────────────────────────
    std::vector<BrokerEntity> get_v1_rebalancing_portfolios(const std::string& query = "");
    BrokerEntity post_v1_rebalancing_portfolios(const json& request);
    BrokerEntity get_v1_rebalancing_portfolio(const std::string& portfolio_id);
    BrokerEntity patch_v1_rebalancing_portfolio(const std::string& portfolio_id, const json& request);
    void delete_v1_rebalancing_portfolio(const std::string& portfolio_id);
    std::vector<BrokerEntity> get_v1_rebalancing_subscriptions(const std::string& query = "");
    BrokerEntity post_v1_rebalancing_subscriptions(const json& request);
    BrokerEntity get_v1_rebalancing_subscription(const std::string& subscription_id);
    void delete_v1_rebalancing_subscription(const std::string& subscription_id);
    std::vector<BrokerEntity> get_v1_rebalancing_runs(const std::string& query = "");
    BrokerEntity post_v1_rebalancing_runs(const json& request);
    BrokerEntity get_v1_rebalancing_run(const std::string& run_id);
    void delete_v1_rebalancing_run(const std::string& run_id);

    // ── Reporting / Country / IRA ────────────────────────────────────────
    BrokerEntity get_v1beta1_logo(const std::string& symbol);
    std::vector<BrokerEntity> get_v1_reporting_eod_aggregate_positions(const std::string& query = "");
    std::vector<BrokerEntity> get_v1_reporting_eod_positions(const std::string& query = "");
    std::vector<BrokerEntity> get_v1_reporting_eod_cash_interest(const std::string& query = "");
    std::vector<BrokerEntity> query_country_infos(const std::string& query = "");
    std::vector<BrokerEntity> list_ira_excess_contributions(const std::string& query = "");

    // ── Crypto Wallets (Account-scoped) ──────────────────────────────────
    std::vector<CryptoWallet> list_crypto_funding_wallets_for_account(const std::string& account_id);
    std::vector<CryptoTransfer> list_crypto_funding_transfers_for_account(const std::string& account_id, const std::string& asset = "");
    CryptoTransfer create_crypto_transfer_for_account(const std::string& account_id, const CryptoTransferRequest& request);
    CryptoTransfer get_crypto_funding_transfer_for_account(const std::string& account_id, const std::string& transfer_id);
    std::vector<WhitelistedAddress> list_whitelisted_addresses_for_account(const std::string& account_id);
    WhitelistedAddress create_whitelisted_address_for_account(const std::string& account_id, const WhitelistedAddressRequest& request);
    void delete_whitelisted_address_for_account(const std::string& account_id, const std::string& whitelisted_address_id);
    CryptoTransferEstimate get_crypto_transfer_estimate(const std::string& asset, const std::string& network = "", const Decimal& amount = Decimal{});

};

}