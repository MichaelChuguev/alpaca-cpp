#include "alpaca/api/rest/trader/AlpacaTraderAPI.h"
#include "alpaca/core/error.h"
#include "alpaca/core/util.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Helper: convert vector of JSON objects into a typed vector
// ---------------------------------------------------------------------------
template <typename T>
static std::vector<T> parse_array(const json& j) {
    std::vector<T> out;
    out.reserve(j.size());
    for (const auto& item : j) {
        out.emplace_back(item);
    }
    return out;
}

// ---------------------------------------------------------------------------
// Account
// ---------------------------------------------------------------------------

Account AlpacaTraderAPI::get_account() {
    return Account(httpClient.get("/v2/account"));
}

AccountConfig AlpacaTraderAPI::get_account_config() {
    return AccountConfig(httpClient.get("/v2/account/configurations"));
}

AccountConfig AlpacaTraderAPI::send_account_config(const AccountConfig& account_config) {
    return AccountConfig(httpClient.patch("/v2/account/configurations", account_config.to_json()));
}

// ---------------------------------------------------------------------------
// Assets
// ---------------------------------------------------------------------------

std::vector<Asset> AlpacaTraderAPI::get_assets(const std::string& status, const std::string& asset_class, const std::string& exchange, const std::vector<std::string>& attributes) {
    auto query = QueryBuilder("/v2/assets")
        .add("status", status)
        .add("asset_class", asset_class)
        .add("exchange", exchange)
        .add_list("attributes", attributes)
        .build();

    return parse_array<Asset>(httpClient.get(query));
}

Asset AlpacaTraderAPI::get_asset(const std::string& symbol_id) {
    return Asset(httpClient.get("/v2/assets/" + symbol_id));
}

// ---------------------------------------------------------------------------
// Options
// ---------------------------------------------------------------------------

std::vector<OptionContract> AlpacaTraderAPI::get_option_contracts(const std::vector<std::string>& underlying_symbols, bool show_deliverables, OptionContractStatus status, const DateTime& expiration_date, const DateTime& expiration_date_gte, const DateTime& expiration_date_lte, const std::string& root_symbol, OptionContractType type, OptionContractStyle style, const Decimal& strike_price_gte, const Decimal& strike_price_lte, const std::string& page_token, int limit, bool ppind) {
    auto qb = QueryBuilder("/v2/options/contracts")
        .add_list("underlying_symbols", underlying_symbols)
        .add("status", status == OptionContractStatus::ACTIVE ? "active" : "inactive")
        .add("root_symbol", root_symbol)
        .add("type", type == OptionContractType::CALL ? "call" : "put")
        .add("style", style == OptionContractStyle::AMERICAN ? "american" : "european")
        .add("strike_price_gte", strike_price_gte)
        .add("strike_price_lte", strike_price_lte)
        .add("limit", limit);

    if (show_deliverables) qb.add("show_deliverables", true);
    if (ppind) qb.add("ppind", true);
    if (expiration_date.is_date_only()) qb.add("expiration_date", expiration_date.to_iso_string());
    if (expiration_date_gte.is_date_only()) qb.add("expiration_date_gte", expiration_date_gte.to_iso_string());
    if (expiration_date_lte.is_date_only()) qb.add("expiration_date_lte", expiration_date_lte.to_iso_string());

    std::string base_query = qb.build();
    std::vector<OptionContract> all_contracts;
    std::string next_page_token = page_token;

    do {
        std::string paginated_query = base_query;
        if (!next_page_token.empty()) {
            paginated_query += (paginated_query.find('?') == std::string::npos ? "?" : "&");
            paginated_query += "page_token=" + next_page_token;
        }

        json j = httpClient.get(paginated_query);
        for (const auto& contract : j["contracts"]) {
            all_contracts.emplace_back(contract);
        }

        next_page_token = j.contains("next_page_token")
            ? j["next_page_token"].get<std::string>()
            : "";
    } while (!next_page_token.empty());

    return all_contracts;
}

OptionContract AlpacaTraderAPI::get_option_contract(const std::string& symbol_id) {
    return OptionContract(httpClient.get("/v2/options/contracts/" + symbol_id));
}

void AlpacaTraderAPI::exercise_option(const std::string& symbol_contract_id) {
    httpClient.post("/v2/positions/" + symbol_contract_id + "/exercise", json::object());
}

void AlpacaTraderAPI::do_not_exercise_option(const std::string& symbol_contract_id) {
    httpClient.post("/v2/positions/" + symbol_contract_id + "/do-not-exercise", json::object());
}

// ---------------------------------------------------------------------------
// Treasuries
// ---------------------------------------------------------------------------

std::vector<Treasury> AlpacaTraderAPI::get_us_treasuries(const Subtype& subtype, const BondStatus& bond_status, const std::vector<std::string>& cusips, const std::vector<std::string>& isins) {
    auto query = QueryBuilder("/v2/assets/fixed_income/us_treasuries")
        .add("subtype", subtype_to_string(subtype))
        .add("bond_status", bond_status_to_string(bond_status))
        .add_list("cusips", cusips)
        .add_list("isins", isins)
        .build();

    json j = httpClient.get(query);
    return parse_array<Treasury>(j["treasuries"]);
}

// ---------------------------------------------------------------------------
// Orders
// ---------------------------------------------------------------------------

OrderResponse AlpacaTraderAPI::create_order(const Order& order) {
    return OrderResponse(httpClient.post("/v2/orders", order.to_json()));
}

std::vector<OrderResponse> AlpacaTraderAPI::get_orders(OrderStatus status, int limit, const DateTime& after, const DateTime& until, Sort direction, bool nested, const std::vector<std::string>& symbols, OrderSide side) {
    std::string status_str;
    if (status == OrderStatus::OPEN)   status_str = "open";
    else if (status == OrderStatus::CLOSED) status_str = "closed";
    else if (status == OrderStatus::ALL)    status_str = "all";

    auto qb = QueryBuilder("/v2/orders")
        .add("status", status_str)
        .add("limit", limit)
        .add("direction", sort_to_string(direction))
        .add_list("symbols", symbols);

    if (after.is_date_only()) qb.add("after", after.to_iso_string());
    if (until.is_date_only()) qb.add("until", until.to_iso_string());
    if (nested) qb.add("nested", true);

    json j = httpClient.get(qb.build());
    std::vector<OrderResponse> orders;
    if (j.is_array()) {
        orders = parse_array<OrderResponse>(j);
    }
    return orders;
}

std::map<std::string, int> AlpacaTraderAPI::delete_all_orders() {
    json j = httpClient.del("/v2/orders");
    std::map<std::string, int> result;
    for (const auto& item : j) {
        result[item["id"].get<std::string>()] = item["status"].get<int>();
    }
    return result;
}

OrderResponse AlpacaTraderAPI::get_order_by_client_order_id(const std::string& client_order_id) {
    auto query = QueryBuilder("/v2/orders")
        .add("client_order_id", client_order_id)
        .build();
    return OrderResponse(httpClient.get(query));
}

OrderResponse AlpacaTraderAPI::get_order_by_id(const std::string& order_id, bool nested) {
    std::string endpoint = "/v2/orders/" + order_id;
    if (nested) endpoint += "?nested=true";
    return OrderResponse(httpClient.get(endpoint));
}

OrderResponse AlpacaTraderAPI::replace_order(const std::string& order_id, int qty, OrderTimeInForce time_in_force, const Decimal& limit_price, const Decimal& stop_price, const Decimal& trail, const std::string& client_order_id) {
    json body;
    if (qty > 0) body["qty"] = qty;
    body["time_in_force"] = time_in_force_to_string(time_in_force);
    if (limit_price > 0) body["limit_price"] = limit_price.to_string();
    if (stop_price > 0) body["stop_price"] = stop_price.to_string();
    if (trail > 0) body["trail_price"] = trail.to_string();
    if (!client_order_id.empty()) body["client_order_id"] = client_order_id;
    return OrderResponse(httpClient.patch("/v2/orders/" + order_id, body));
}

void AlpacaTraderAPI::delete_order(const std::string& order_id) {
    httpClient.del("/v2/orders/" + order_id);
}

// ---------------------------------------------------------------------------
// Positions
// ---------------------------------------------------------------------------

std::vector<Position> AlpacaTraderAPI::get_all_positions() {
    return parse_array<Position>(httpClient.get("/v2/positions"));
}

std::vector<PositionClosed> AlpacaTraderAPI::close_all_positions(bool cancel_orders) {
    std::string endpoint = "/v2/positions";
    if (cancel_orders) endpoint += "?cancel_orders=true";
    return parse_array<PositionClosed>(httpClient.del(endpoint));
}

Position AlpacaTraderAPI::get_open_position(const std::string& symbol_id) {
    return Position(httpClient.get("/v2/positions/" + symbol_id));
}

OrderResponse AlpacaTraderAPI::close_position(const std::string& symbol_id, const Decimal& qty, const Decimal& percentage) {
    auto query = QueryBuilder("/v2/positions/" + symbol_id)
        .add("qty", qty)
        .add("percentage", percentage)
        .build();
    return OrderResponse(httpClient.del(query));
}

// ---------------------------------------------------------------------------
// Portfolio
// ---------------------------------------------------------------------------

static std::string intraday_reporting_to_string(IntradayReporting r) {
    switch (r) {
        case IntradayReporting::MARKET_HOURS:   return "market_hours";
        case IntradayReporting::EXTENDED_HOURS: return "extended_hours";
        case IntradayReporting::CONTINUOUS:     return "continuous";
    }
    return "";
}

static std::string pnl_reset_to_string(PnlReset r) {
    return r == PnlReset::PER_DAY ? "per_day" : "no_reset";
}

PortfolioHistory AlpacaTraderAPI::get_portfolio_history(const std::string& period, const std::string& timeframe, IntradayReporting reporting, const DateTime& start, PnlReset reset, const DateTime& end, const std::string& cashflow_types) {
    auto qb = QueryBuilder("/v2/account/portfolio/history")
        .add("period", period)
        .add("timeframe", timeframe)
        .add("intraday_reporting", intraday_reporting_to_string(reporting))
        .add("pnl_reset", pnl_reset_to_string(reset))
        .add("cashflow_types", cashflow_types);

    if (start.to_epoch_seconds() > 0 && start.is_date_only()) qb.add("start", start.to_iso_string());
    if (end.to_epoch_seconds() > 0 && end.is_date_only()) qb.add("end", end.to_iso_string());

    return PortfolioHistory(httpClient.get(qb.build()));
}

// ---------------------------------------------------------------------------
// Watchlists
// ---------------------------------------------------------------------------

std::vector<Watchlist> AlpacaTraderAPI::get_all_watchlists() {
    return parse_array<Watchlist>(httpClient.get("/v2/watchlists"));
}

Watchlist AlpacaTraderAPI::create_watchlist(const std::string& name, const std::vector<std::string>& symbols) {
    json body;
    body["name"] = name;
    body["symbols"] = symbols;
    return Watchlist(httpClient.post("/v2/watchlists", body));
}

Watchlist AlpacaTraderAPI::get_watchlist_by_id(const std::string& id) {
    return Watchlist(httpClient.get("/v2/watchlists/" + id));
}

Watchlist AlpacaTraderAPI::update_watchlist(const std::string& id, const std::string& name, const std::vector<std::string>& symbols) {
    json body;
    body["name"] = name;
    body["symbols"] = symbols;
    return Watchlist(httpClient.put("/v2/watchlists/" + id, body));
}

Watchlist AlpacaTraderAPI::add_asset_to_watchlist(const std::string& id, const std::string& symbol) {
    json body;
    body["symbol"] = symbol;
    return Watchlist(httpClient.post("/v2/watchlists/" + id, body));
}

void AlpacaTraderAPI::delete_watchlist(const std::string& id) {
    httpClient.del("/v2/watchlists/" + id);
}

Watchlist AlpacaTraderAPI::get_watchlist_by_name(const std::string& name) {
    return Watchlist(httpClient.get("/v2/watchlists:by_name?name=" + url_encode(name)));
}

Watchlist AlpacaTraderAPI::update_watchlist_by_name(const std::string& name, const std::vector<std::string>& symbols) {
    json body;
    body["name"] = name;
    body["symbols"] = symbols;
    return Watchlist(httpClient.put("/v2/watchlists:by_name", body));
}

Watchlist AlpacaTraderAPI::add_asset_to_watchlist_by_name(const std::string& name, const std::string& symbol) {
    json body;
    body["name"] = name;
    body["symbol"] = symbol;
    return Watchlist(httpClient.post("/v2/watchlists:by_name", body));
}

void AlpacaTraderAPI::delete_watchlist_by_name(const std::string& name) {
    httpClient.del("/v2/watchlists:by_name?name=" + url_encode(name));
}

Watchlist AlpacaTraderAPI::remove_asset_from_watchlist(const std::string& id, const std::string& symbol) {
    return Watchlist(httpClient.del("/v2/watchlists/" + id + "/" + symbol));
}

// ---------------------------------------------------------------------------
// Activities
// ---------------------------------------------------------------------------

std::vector<AccountActivity> AlpacaTraderAPI::get_account_activities(const DateTime& after, const DateTime& until, Sort direction, int page_size, const std::string& page_token, const std::string& category) {
    auto qb = QueryBuilder("/v2/account/activities")
        .add("direction", sort_to_string(direction))
        .add("page_size", page_size)
        .add("page_token", page_token)
        .add("category", category);

    if (after.is_date_only()) qb.add("after", after.to_iso_string());
    if (until.is_date_only()) qb.add("until", until.to_iso_string());

    return parse_array<AccountActivity>(httpClient.get(qb.build()));
}

std::vector<AccountActivity> AlpacaTraderAPI::get_account_activities_by_type(const std::string& activity_type, const DateTime& after, const DateTime& until, Sort direction, int page_size, const std::string& page_token) {
    auto qb = QueryBuilder("/v2/account/activities/" + activity_type)
        .add("direction", sort_to_string(direction))
        .add("page_size", page_size)
        .add("page_token", page_token);

    if (after.is_date_only()) qb.add("after", after.to_iso_string());
    if (until.is_date_only()) qb.add("until", until.to_iso_string());

    return parse_array<AccountActivity>(httpClient.get(qb.build()));
}

// ---------------------------------------------------------------------------
// Calendar & Clock
// ---------------------------------------------------------------------------

Calendar AlpacaTraderAPI::get_calendar(const DateTime& start, const DateTime& end) {
    auto qb = QueryBuilder("/v2/calendar");
    if (start.is_date_only()) qb.add("start", start.to_iso_string());
    if (end.is_date_only()) qb.add("end", end.to_iso_string());
    return Calendar(httpClient.get(qb.build()));
}

Clock AlpacaTraderAPI::get_clock() {
    return Clock(httpClient.get("/v2/clock"));
}

Calendar AlpacaTraderAPI::get_calendar_v3(const std::string& market, const DateTime& start, const DateTime& end) {
    auto qb = QueryBuilder("/v3/calendar/" + market);
    if (start.is_date_only()) qb.add("start", start.to_iso_string());
    if (end.is_date_only()) qb.add("end", end.to_iso_string());
    return Calendar(httpClient.get(qb.build()));
}

Clock AlpacaTraderAPI::get_clock_v3() {
    return Clock(httpClient.get("/v3/clock"));
}

// ---------------------------------------------------------------------------
// Crypto
// ---------------------------------------------------------------------------

std::vector<CryptoWallet> AlpacaTraderAPI::get_crypto_wallets() {
    return parse_array<CryptoWallet>(httpClient.get("/v2/wallets"));
}

std::vector<CryptoTransfer> AlpacaTraderAPI::get_crypto_transfers(const std::string& asset) {
    auto query = QueryBuilder("/v2/wallets/transfers")
        .add("asset", asset)
        .build();
    return parse_array<CryptoTransfer>(httpClient.get(query));
}

CryptoTransfer AlpacaTraderAPI::create_crypto_transfer(const CryptoTransferRequest& request) {
    return CryptoTransfer(httpClient.post("/v2/wallets/transfers", request.to_json()));
}

CryptoTransfer AlpacaTraderAPI::get_crypto_transfer(const std::string& transfer_id) {
    return CryptoTransfer(httpClient.get("/v2/wallets/transfers/" + transfer_id));
}

std::vector<WhitelistedAddress> AlpacaTraderAPI::get_whitelisted_addresses() {
    return parse_array<WhitelistedAddress>(httpClient.get("/v2/wallets/whitelists"));
}

WhitelistedAddress AlpacaTraderAPI::create_whitelisted_address(const WhitelistedAddressRequest& request) {
    return WhitelistedAddress(httpClient.post("/v2/wallets/whitelists", request.to_json()));
}

void AlpacaTraderAPI::delete_whitelisted_address(const std::string& whitelisted_address_id) {
    httpClient.del("/v2/wallets/whitelists/" + whitelisted_address_id);
}

CryptoTransferEstimate AlpacaTraderAPI::get_crypto_transfer_estimate(const std::string& asset, const std::string& network, const Decimal& amount) {
    auto query = QueryBuilder("/v2/wallets/fees/estimate")
        .add("asset", asset)
        .add("network", network)
        .add("amount", amount)
        .build();
    return CryptoTransferEstimate(httpClient.get(query));
}

} // namespace alpaca