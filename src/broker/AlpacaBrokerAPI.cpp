#include "alpaca/AlpacaBrokerAPI.h"

#include "alpaca/core/util.h"

namespace alpaca {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

template <typename T>
static std::vector<T> parse_array(const json& j) {
    std::vector<T> out;
    if (!j.is_array()) return out;
    out.reserve(j.size());
    for (const auto& item : j) {
        out.emplace_back(item);
    }
    return out;
}

static std::string with_query(const std::string& path, const std::string& query) {
    if (query.empty()) return path;
    if (query[0] == '?') return path + query;
    return path + "?" + query;
}

static std::string order_status_to_string(OrderStatus status) {
    switch (status) {
        case OrderStatus::OPEN: return "open";
        case OrderStatus::CLOSED: return "closed";
        case OrderStatus::ALL: return "all";
    }
    return "open";
}

static std::string intraday_reporting_to_string(IntradayReporting r) {
    switch (r) {
        case IntradayReporting::MARKET_HOURS: return "market_hours";
        case IntradayReporting::EXTENDED_HOURS: return "extended_hours";
        case IntradayReporting::CONTINUOUS: return "continuous";
    }
    return "";
}

static std::string pnl_reset_to_string(PnlReset r) {
    return r == PnlReset::PER_DAY ? "per_day" : "no_reset";
}

std::string AlpacaBrokerAPI::broker_host() const {
    return brokerAPIEndpoint == BrokerAPIEndpoint::SANDBOX
        ? "broker-api.sandbox.alpaca.markets"
        : "broker-api.alpaca.markets";
}

std::string AlpacaBrokerAPI::make_sse_url(const std::string& path) const {
    return "https://" + broker_host() + path;
}

// ---------------------------------------------------------------------------
// Accounts
// ---------------------------------------------------------------------------

std::vector<BrokerAccount> AlpacaBrokerAPI::get_accounts(const std::string& status, const std::string& created_after, const std::string& created_before, const std::string& sort, const std::string& entities) {
    std::string query;
    if (!status.empty()) query += "status=" + status + "&";
    if (!created_after.empty()) query += "created_after=" + created_after + "&";
    if (!created_before.empty()) query += "created_before=" + created_before + "&";
    if (!sort.empty()) query += "sort=" + sort + "&";
    if (!entities.empty()) query += "entities=" + entities + "&";
    if (!query.empty() && query.back() == '&') query.pop_back();
    return parse_array<BrokerAccount>(httpClient.get(with_query("/v1/accounts", query)));
}

BrokerAccount AlpacaBrokerAPI::create_account(const json& account_request) {
    return BrokerAccount(httpClient.post("/v1/accounts", account_request));
}

BrokerAccount AlpacaBrokerAPI::get_account(const std::string& account_id) {
    return BrokerAccount(httpClient.get("/v1/accounts/" + account_id));
}

BrokerAccount AlpacaBrokerAPI::patch_account(const std::string& account_id, const json& patch_request) {
    return BrokerAccount(httpClient.patch("/v1/accounts/" + account_id, patch_request));
}

OptionsApproval AlpacaBrokerAPI::request_options_for_account(const std::string& account_id, int level) {
    return OptionsApproval(httpClient.post("/v1/accounts/" + account_id + "/options/approval", {{"level", level}}));
}

std::vector<OptionsApproval> AlpacaBrokerAPI::get_options_approvals_requests(const std::string& account_id, int requested_level, int approved_level, OptionsApprovalStatus status, int page_size, const std::string& page_token) {
    auto qb = QueryBuilder("/v1/accounts/options/approvals")
        .add("account_id", account_id)
        .add("status", options_approval_status_to_string(status))
        .add("page_token", page_token);

    if (requested_level >= 0) qb.add("requested_level", std::to_string(requested_level));
    if (approved_level >= 0) qb.add("approved_level", std::to_string(approved_level));
    if (page_size > 0) qb.add("page_size", page_size);

    json j = httpClient.get(qb.build());
    return parse_array<OptionsApproval>(j["options_approvals"]);
}

std::vector<BrokerAccountActivity> AlpacaBrokerAPI::get_account_activities(const std::string& account_id, const std::vector<ActivityType>& activity_types, ActivityCategory category, const DateTime& date, const DateTime& until, const DateTime& after, Sort direction, int page_size, const std::string& page_token) {
    auto qb = QueryBuilder("/v1/accounts/activities")
        .add("account_id", account_id)
        .add("direction", sort_to_string(direction))
        .add("page_token", page_token);

    if (!activity_types.empty()) {
        std::vector<std::string> type_strings;
        type_strings.reserve(activity_types.size());
        for (const auto& t : activity_types) type_strings.push_back(activity_type_to_string(t));
        qb.add_list("activity_types", type_strings);
    }

    if (category != ActivityCategory::UNSET) qb.add("category", activity_category_to_string(category));
    if (page_size > 0) qb.add("page_size", page_size);
    if (date.is_date_only()) qb.add("date", date.to_iso_string());
    if (until.is_date_only()) qb.add("until", until.to_iso_string());
    if (after.is_date_only()) qb.add("after", after.to_iso_string());

    return parse_array<BrokerAccountActivity>(httpClient.get(qb.build()));
}

std::vector<BrokerAccountActivity> AlpacaBrokerAPI::get_account_activities_by_type(ActivityType activity_type, const std::string& account_id, const DateTime& date, const DateTime& until, const DateTime& after, Sort direction, int page_size, const std::string& page_token) {
    auto qb = QueryBuilder("/v1/accounts/activities/" + activity_type_to_string(activity_type))
        .add("account_id", account_id)
        .add("direction", sort_to_string(direction))
        .add("page_token", page_token);

    if (page_size > 0) qb.add("page_size", page_size);
    if (date.is_date_only()) qb.add("date", date.to_iso_string());
    if (until.is_date_only()) qb.add("until", until.to_iso_string());
    if (after.is_date_only()) qb.add("after", after.to_iso_string());

    return parse_array<BrokerAccountActivity>(httpClient.get(qb.build()));
}

Account AlpacaBrokerAPI::get_trading_account(const std::string& account_id) {
    return Account(httpClient.get("/v1/trading/accounts/" + account_id + "/account"));
}

PDTStatus AlpacaBrokerAPI::get_pdt_status(const std::string& account_id) {
    return PDTStatus(httpClient.get("/v1/trading/accounts/" + account_id + "/account/pdt/status"));
}

PDTStatus AlpacaBrokerAPI::pdt_one_time_removal(const std::string& account_id) {
    return PDTStatus(httpClient.post("/v1/trading/accounts/" + account_id + "/account/pdt/one-time-removal", json::object()));
}

BrokerEntity AlpacaBrokerAPI::close_account(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1/accounts/" + account_id + "/actions/close", request));
}

// ---------------------------------------------------------------------------
// Account Documents
// ---------------------------------------------------------------------------

std::vector<BrokerAccountDocument> AlpacaBrokerAPI::get_docs_for_account(const std::string& account_id, BrokerAccountDocumentType type, const DateTime& start, const DateTime& end, const std::string& page_token) {
    auto qb = QueryBuilder("/v1/accounts/" + account_id + "/documents")
        .add("type", broker_account_document_type_to_string(type))
        .add("page_token", page_token);

    if (start.to_epoch_seconds() > 0) qb.add("start", start.to_iso_string());
    if (end.to_epoch_seconds() > 0) qb.add("end", end.to_iso_string());

    return parse_array<BrokerAccountDocument>(httpClient.get(qb.build()));
}

std::vector<BrokerAccountDocument> AlpacaBrokerAPI::upload_doc_to_account(const std::string& account_id, const std::vector<BrokerDocumentUploadRequest>& documents) {
    json body = json::array();
    for (const auto& doc : documents) body.push_back(doc.to_json());
    return parse_array<BrokerAccountDocument>(httpClient.post("/v1/accounts/" + account_id + "/documents/upload", body));
}

BrokerAccountDocument AlpacaBrokerAPI::download_doc_from_account(const std::string& account_id, const std::string& document_id) {
    return BrokerAccountDocument(httpClient.get("/v1/accounts/" + account_id + "/documents/" + document_id + "/download"));
}

BrokerAccountDocument AlpacaBrokerAPI::download_w8ben_doc_from_account(const std::string& account_id, const std::string& document_id) {
    return BrokerAccountDocument(httpClient.get("/v1/accounts/" + account_id + "/documents/w8ben/" + document_id + "/download"));
}

// ---------------------------------------------------------------------------
// Trading: Positions
// ---------------------------------------------------------------------------

std::vector<Position> AlpacaBrokerAPI::get_positions_for_account(const std::string& account_id) {
    return parse_array<Position>(httpClient.get("/v1/trading/accounts/" + account_id + "/positions"));
}

std::vector<PositionClosed> AlpacaBrokerAPI::close_all_positions_for_account(const std::string& account_id, bool cancel_orders) {
    std::string endpoint = "/v1/trading/accounts/" + account_id + "/positions";
    if (cancel_orders) endpoint += "?cancel_orders=true";
    return parse_array<PositionClosed>(httpClient.del(endpoint));
}

Position AlpacaBrokerAPI::get_position_for_account_by_symbol(const std::string& account_id, const std::string& symbol_or_asset_id) {
    return Position(httpClient.get("/v1/trading/accounts/" + account_id + "/positions/" + symbol_or_asset_id));
}

OrderResponse AlpacaBrokerAPI::close_position_for_account_by_symbol(const std::string& account_id, const std::string& symbol_or_asset_id, const Decimal& qty, const Decimal& percentage) {
    auto qb = QueryBuilder("/v1/trading/accounts/" + account_id + "/positions/" + symbol_or_asset_id)
        .add("qty", qty)
        .add("percentage", percentage);

    return OrderResponse(httpClient.del(qb.build()));
}

void AlpacaBrokerAPI::option_exercise_for_account(const std::string& account_id, const std::string& symbol_or_contract_id) {
    httpClient.post("/v1/trading/accounts/" + account_id + "/positions/" + symbol_or_contract_id + "/exercise", json::object());
}

void AlpacaBrokerAPI::option_do_not_exercise_for_account(const std::string& account_id, const std::string& symbol_or_contract_id) {
    httpClient.post("/v1/trading/accounts/" + account_id + "/positions/" + symbol_or_contract_id + "/do-not-exercise", json::object());
}

// ---------------------------------------------------------------------------
// Trading: Orders
// ---------------------------------------------------------------------------

OrderResponse AlpacaBrokerAPI::get_order_for_account(const std::string& account_id, const std::string& order_id, bool nested) {
    std::string endpoint = "/v1/trading/accounts/" + account_id + "/orders/" + order_id;
    if (nested) endpoint += "?nested=true";
    return OrderResponse(httpClient.get(endpoint));
}

OrderResponse AlpacaBrokerAPI::replace_order_for_account(const std::string& account_id, const std::string& order_id, const ReplaceOrder& replace_request) {
    return OrderResponse(httpClient.patch("/v1/trading/accounts/" + account_id + "/orders/" + order_id, replace_request.to_json()));
}

void AlpacaBrokerAPI::delete_order_for_account(const std::string& account_id, const std::string& order_id) {
    httpClient.del("/v1/trading/accounts/" + account_id + "/orders/" + order_id);
}

std::vector<OrderResponse> AlpacaBrokerAPI::get_all_orders_for_account(const std::string& account_id, OrderStatus status, int limit, const DateTime& after, const DateTime& until, Sort direction, bool nested, const std::vector<std::string>& symbols) {
    auto qb = QueryBuilder("/v1/trading/accounts/" + account_id + "/orders")
        .add("status", order_status_to_string(status))
        .add("limit", limit)
        .add("direction", sort_to_string(direction))
        .add_list("symbols", symbols);

    if (after.is_date_only()) qb.add("after", after.to_iso_string());
    if (until.is_date_only()) qb.add("until", until.to_iso_string());
    if (nested) qb.add("nested", true);

    return parse_array<OrderResponse>(httpClient.get(qb.build()));
}

OrderResponse AlpacaBrokerAPI::get_order_by_client_order_id_for_account(const std::string& account_id, const std::string& client_order_id) {
    auto query = QueryBuilder("/v1/trading/accounts/" + account_id + "/orders:by_client_order_id")
        .add("client_order_id", client_order_id)
        .build();

    return OrderResponse(httpClient.get(query));
}

OrderResponse AlpacaBrokerAPI::create_order_for_account(const std::string& account_id, const Order& order) {
    return OrderResponse(httpClient.post("/v1/trading/accounts/" + account_id + "/orders", order.to_json()));
}

std::map<std::string, int> AlpacaBrokerAPI::delete_all_orders_for_account(const std::string& account_id) {
    json j = httpClient.del("/v1/trading/accounts/" + account_id + "/orders");
    std::map<std::string, int> result;
    for (const auto& item : j) {
        result[item["id"].get<std::string>()] = item["status"].get<int>();
    }
    return result;
}

OrderResponse AlpacaBrokerAPI::get_order_estimation_for_account(const std::string& account_id, const Order& order) {
    return OrderResponse(httpClient.post("/v1/trading/accounts/" + account_id + "/orders/estimation", order.to_json()));
}

// ---------------------------------------------------------------------------
// Trading: Account Config / Limits / Portfolio
// ---------------------------------------------------------------------------

AccountConfig AlpacaBrokerAPI::patch_trading_account_configurations(const std::string& account_id, const AccountConfig& config) {
    return AccountConfig(httpClient.patch("/v1/trading/accounts/" + account_id + "/account/configurations", config.to_json()));
}

BrokerEntity AlpacaBrokerAPI::get_account_trading_limits(const std::string& account_id) {
    return BrokerEntity(httpClient.get("/v1/trading/accounts/" + account_id + "/limits"));
}

PortfolioHistory AlpacaBrokerAPI::get_portfolio_history_for_account(const std::string& account_id, const std::string& period, const std::string& timeframe, IntradayReporting reporting, const DateTime& start, PnlReset reset, const DateTime& end, const std::string& cashflow_types) {
    auto qb = QueryBuilder("/v1/trading/accounts/" + account_id + "/account/portfolio/history")
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
// Instant Funding
// ---------------------------------------------------------------------------

std::vector<BrokerEntity> AlpacaBrokerAPI::get_instant_funding_list(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/instant_funding", query)));
}

BrokerEntity AlpacaBrokerAPI::create_instant_funding(const json& request) {
    return BrokerEntity(httpClient.post("/v1/instant_funding", request));
}

BrokerEntity AlpacaBrokerAPI::get_instant_funding_single(const std::string& instant_funding_id) {
    return BrokerEntity(httpClient.get("/v1/instant_funding/" + instant_funding_id));
}

void AlpacaBrokerAPI::delete_instant_funding_single(const std::string& instant_funding_id) {
    httpClient.del("/v1/instant_funding/" + instant_funding_id);
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_instant_funding_settlements(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/instant_funding/settlements", query)));
}

BrokerEntity AlpacaBrokerAPI::create_instant_funding_settlement(const json& request) {
    return BrokerEntity(httpClient.post("/v1/instant_funding/settlements", request));
}

BrokerEntity AlpacaBrokerAPI::get_instant_funding_settlement_single(const std::string& settlement_id) {
    return BrokerEntity(httpClient.get("/v1/instant_funding/settlements/" + settlement_id));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_instant_funding_correspondent_limits(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/instant_funding/limits", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_instant_funding_account_limits(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/instant_funding/limits/accounts", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_instant_funding_reports(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/instant_funding/reports", query)));
}

// ---------------------------------------------------------------------------
// Broker Assets / Options / Treasuries / Clock / Calendar
// ---------------------------------------------------------------------------

std::vector<Asset> AlpacaBrokerAPI::get_assets(const std::string& status, const std::string& asset_class, const std::string& exchange, const std::vector<std::string>& attributes) {
    auto query = QueryBuilder("/v1/assets")
        .add("status", status)
        .add("asset_class", asset_class)
        .add("exchange", exchange)
        .add_list("attributes", attributes)
        .build();

    return parse_array<Asset>(httpClient.get(query));
}

Asset AlpacaBrokerAPI::get_asset(const std::string& symbol_or_asset_id) {
    return Asset(httpClient.get("/v1/assets/" + symbol_or_asset_id));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_asset_entry_requirements(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/assets/entry-requirements", query)));
}

std::vector<OptionContract> AlpacaBrokerAPI::get_option_contracts(const std::vector<std::string>& underlying_symbols, bool show_deliverables, OptionContractStatus status, const DateTime& expiration_date, const DateTime& expiration_date_gte, const DateTime& expiration_date_lte, const std::string& root_symbol, OptionContractType type, OptionContractStyle style, const Decimal& strike_price_gte, const Decimal& strike_price_lte, const std::string& page_token, int limit, bool ppind) {
    auto qb = QueryBuilder("/v1/options/contracts")
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

OptionContract AlpacaBrokerAPI::get_option_contract(const std::string& symbol_or_id) {
    return OptionContract(httpClient.get("/v1/options/contracts/" + symbol_or_id));
}

std::vector<Treasury> AlpacaBrokerAPI::get_us_treasuries(const Subtype& subtype, const BondStatus& bond_status, const std::vector<std::string>& cusips, const std::vector<std::string>& isins) {
    auto query = QueryBuilder("/v1/assets/fixed_income/us_treasuries")
        .add("subtype", subtype_to_string(subtype))
        .add("bond_status", bond_status_to_string(bond_status))
        .add_list("cusips", cusips)
        .add_list("isins", isins)
        .build();

    json j = httpClient.get(query);
    return parse_array<Treasury>(j["treasuries"]);
}

Calendar AlpacaBrokerAPI::get_calendar(const DateTime& start, const DateTime& end) {
    auto qb = QueryBuilder("/v1/calendar");
    if (start.is_date_only()) qb.add("start", start.to_iso_string());
    if (end.is_date_only()) qb.add("end", end.to_iso_string());
    return Calendar(httpClient.get(qb.build()));
}

Calendar AlpacaBrokerAPI::get_calendar_v2(const std::string& market, const DateTime& start, const DateTime& end) {
    auto qb = QueryBuilder("/v2/calendar/" + market);
    if (start.is_date_only()) qb.add("start", start.to_iso_string());
    if (end.is_date_only()) qb.add("end", end.to_iso_string());
    return Calendar(httpClient.get(qb.build()));
}

Clock AlpacaBrokerAPI::get_clock() {
    return Clock(httpClient.get("/v1/clock"));
}

Clock AlpacaBrokerAPI::get_clock_v2() {
    return Clock(httpClient.get("/v2/clock"));
}

// ---------------------------------------------------------------------------
// Events (SSE)
// ---------------------------------------------------------------------------

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_account_status_sse() const {
    return BrokerSSESubscription(make_sse_url("/v1/events/accounts/status"));
}

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_journal_status_sse() const {
    return BrokerSSESubscription(make_sse_url("/v1/events/journals/status"));
}

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_journal_status_v2_sse() const {
    return BrokerSSESubscription(make_sse_url("/v2/events/journals/status"));
}

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_trade_v2_sse() const {
    return BrokerSSESubscription(make_sse_url("/v2/events/trades"));
}

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_admin_action_sse() const {
    return BrokerSSESubscription(make_sse_url("/v2/events/admin-actions"));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_events_nta(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/events/nta", query)));
}

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_funding_status_sse() const {
    return BrokerSSESubscription(make_sse_url("/v2/events/funding/status"));
}

BrokerSSESubscription AlpacaBrokerAPI::subscribe_to_system_event_v2_sse() const {
    return BrokerSSESubscription(make_sse_url("/v2/events/system"));
}

BrokerSSEClient AlpacaBrokerAPI::create_sse_client(const BrokerSSESubscription& subscription) const {
    if (brokerAuthMode == BrokerAuthMode::CLIENT_CREDENTIALS) {
        // Provide a token supplier so every (re)connect gets a fresh Bearer token.
        return BrokerSSEClient(subscription.stream_url,
                               [this]() { return httpClient.get_broker_bearer_token(); });
    }
    return BrokerSSEClient(subscription.stream_url, brokerAPIKey, brokerAPISecret);
}

BrokerSSEClient AlpacaBrokerAPI::create_account_status_sse_client() const {
    return create_sse_client(subscribe_to_account_status_sse());
}

BrokerSSEClient AlpacaBrokerAPI::create_journal_status_sse_client() const {
    return create_sse_client(subscribe_to_journal_status_sse());
}

BrokerSSEClient AlpacaBrokerAPI::create_journal_status_v2_sse_client() const {
    return create_sse_client(subscribe_to_journal_status_v2_sse());
}

BrokerSSEClient AlpacaBrokerAPI::create_trade_v2_sse_client() const {
    return create_sse_client(subscribe_to_trade_v2_sse());
}

BrokerSSEClient AlpacaBrokerAPI::create_admin_action_sse_client() const {
    return create_sse_client(subscribe_to_admin_action_sse());
}

BrokerSSEClient AlpacaBrokerAPI::create_funding_status_sse_client() const {
    return create_sse_client(subscribe_to_funding_status_sse());
}

BrokerSSEClient AlpacaBrokerAPI::create_system_event_v2_sse_client() const {
    return create_sse_client(subscribe_to_system_event_v2_sse());
}

// ---------------------------------------------------------------------------
// Funding: Recipient Banks / Transfers / ACH / JIT
// ---------------------------------------------------------------------------

std::vector<BrokerEntity> AlpacaBrokerAPI::get_recipient_banks(const std::string& account_id) {
    return parse_array<BrokerEntity>(httpClient.get("/v1/accounts/" + account_id + "/recipient_banks"));
}

BrokerEntity AlpacaBrokerAPI::create_recipient_bank(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1/accounts/" + account_id + "/recipient_banks", request));
}

void AlpacaBrokerAPI::delete_recipient_bank(const std::string& account_id, const std::string& bank_id) {
    httpClient.del("/v1/accounts/" + account_id + "/recipient_banks/" + bank_id);
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_transfers_for_account(const std::string& account_id, const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/accounts/" + account_id + "/transfers", query)));
}

BrokerEntity AlpacaBrokerAPI::create_transfer_for_account(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1/accounts/" + account_id + "/transfers", request));
}

void AlpacaBrokerAPI::delete_transfer(const std::string& account_id, const std::string& transfer_id) {
    httpClient.del("/v1/accounts/" + account_id + "/transfers/" + transfer_id);
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_account_ach_relationships(const std::string& account_id, const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/accounts/" + account_id + "/ach_relationships", query)));
}

BrokerEntity AlpacaBrokerAPI::create_ach_relationship_for_account(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1/accounts/" + account_id + "/ach_relationships", request));
}

void AlpacaBrokerAPI::delete_ach_relationship_from_account(const std::string& account_id, const std::string& ach_relationship_id) {
    httpClient.del("/v1/accounts/" + account_id + "/ach_relationships/" + ach_relationship_id);
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_transfers_jit_reports(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/transfers/jit/reports", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_transfers_jit_limits(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/transfers/jit/limits", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_transfers_jit_ledgers(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/transfers/jit/ledgers", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_transfers_jit_ledger_balances(const std::string& ledger_id, const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/transfers/jit/" + ledger_id + "/balances", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_jit_settlements(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/jit/settlements", query)));
}

BrokerEntity AlpacaBrokerAPI::post_v1_jit_settlements(const json& request) {
    return BrokerEntity(httpClient.post("/v1/jit/settlements", request));
}

BrokerEntity AlpacaBrokerAPI::get_v1_jit_settlement_single(const std::string& settlement_id) {
    return BrokerEntity(httpClient.get("/v1/jit/settlements/" + settlement_id));
}

// ---------------------------------------------------------------------------
// Cash Interest / OAuth / Journals / FPSL
// ---------------------------------------------------------------------------

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_list_apr_tiers(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/cash_interest/apr_tiers", query)));
}

BrokerEntity AlpacaBrokerAPI::get_oauth_client(const std::string& client_id) {
    return BrokerEntity(httpClient.get("/v1/oauth/clients/" + client_id));
}

BrokerEntity AlpacaBrokerAPI::issue_oauth_token(const std::vector<std::pair<std::string, std::string>>& form_fields) {
    return BrokerEntity(httpClient.post_form("/v1/oauth/token", form_fields));
}

BrokerEntity AlpacaBrokerAPI::authorize_oauth_token(const std::string& query) {
    return BrokerEntity(httpClient.get(with_query("/v1/oauth/authorize", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_all_journals(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/journals", query)));
}

BrokerEntity AlpacaBrokerAPI::create_journal(const json& request) {
    return BrokerEntity(httpClient.post("/v1/journals", request));
}

void AlpacaBrokerAPI::delete_journal_by_id(const std::string& journal_id) {
    httpClient.del("/v1/journals/" + journal_id);
}

BrokerEntity AlpacaBrokerAPI::get_v1_journal(const std::string& journal_id) {
    return BrokerEntity(httpClient.get("/v1/journals/" + journal_id));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::create_batch_journal(const json& request) {
    return parse_array<BrokerEntity>(httpClient.post("/v1/journals/batch", request));
}

BrokerEntity AlpacaBrokerAPI::reverse_batch_journal(const json& request) {
    return BrokerEntity(httpClient.post("/v1/journals/reverse_batch", request));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_list_fpsl_tiers(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/fpsl/tiers", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_list_fpsl_loans(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/fpsl/loans", query)));
}

// ---------------------------------------------------------------------------
// Trading Watchlists
// ---------------------------------------------------------------------------

std::vector<Watchlist> AlpacaBrokerAPI::get_all_watchlists_for_account(const std::string& account_id) {
    return parse_array<Watchlist>(httpClient.get("/v1/trading/accounts/" + account_id + "/watchlists"));
}

Watchlist AlpacaBrokerAPI::create_watchlist_for_account(const std::string& account_id, const std::string& name, const std::vector<std::string>& symbols) {
    json body;
    body["name"] = name;
    body["symbols"] = symbols;
    return Watchlist(httpClient.post("/v1/trading/accounts/" + account_id + "/watchlists", body));
}

Watchlist AlpacaBrokerAPI::get_watchlist_for_account_by_id(const std::string& account_id, const std::string& watchlist_id) {
    return Watchlist(httpClient.get("/v1/trading/accounts/" + account_id + "/watchlists/" + watchlist_id));
}

Watchlist AlpacaBrokerAPI::replace_watchlist_for_account_by_id(const std::string& account_id, const std::string& watchlist_id, const std::string& name, const std::vector<std::string>& symbols) {
    json body;
    body["name"] = name;
    body["symbols"] = symbols;
    return Watchlist(httpClient.put("/v1/trading/accounts/" + account_id + "/watchlists/" + watchlist_id, body));
}

void AlpacaBrokerAPI::delete_watchlist_from_account_by_id(const std::string& account_id, const std::string& watchlist_id) {
    httpClient.del("/v1/trading/accounts/" + account_id + "/watchlists/" + watchlist_id);
}

Watchlist AlpacaBrokerAPI::add_asset_to_watchlist_for_account(const std::string& account_id, const std::string& watchlist_id, const std::string& symbol) {
    json body;
    body["symbol"] = symbol;
    return Watchlist(httpClient.post("/v1/trading/accounts/" + account_id + "/watchlists/" + watchlist_id, body));
}

Watchlist AlpacaBrokerAPI::remove_asset_from_watchlist_for_account(const std::string& account_id, const std::string& watchlist_id, const std::string& symbol) {
    return Watchlist(httpClient.del("/v1/trading/accounts/" + account_id + "/watchlists/" + watchlist_id + "/" + symbol));
}

// ---------------------------------------------------------------------------
// KYC / Identity
// ---------------------------------------------------------------------------

BrokerEntity AlpacaBrokerAPI::post_v1_accounts_cip(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1/accounts/" + account_id + "/cip", request));
}

BrokerEntity AlpacaBrokerAPI::get_v1_accounts_cip(const std::string& account_id) {
    return BrokerEntity(httpClient.get("/v1/accounts/" + account_id + "/cip"));
}

BrokerEntity AlpacaBrokerAPI::get_v1_accounts_onfido_sdk_tokens(const std::string& account_id, const std::string& query) {
    return BrokerEntity(httpClient.get(with_query("/v1/accounts/" + account_id + "/onfido/sdk/tokens", query)));
}

BrokerEntity AlpacaBrokerAPI::patch_v1_accounts_onfido_sdk(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.patch("/v1/accounts/" + account_id + "/onfido/sdk", request));
}

// ---------------------------------------------------------------------------
// Banking Demo / Funding Wallet
// ---------------------------------------------------------------------------

BrokerEntity AlpacaBrokerAPI::demo_deposit_funding(const json& request) {
    return BrokerEntity(httpClient.post("/v1beta/demo/banking/funding", request));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::batch_create_funding_wallets(const json& request) {
    return parse_array<BrokerEntity>(httpClient.post("/v1beta/accounts/funding_wallet", request));
}

BrokerEntity AlpacaBrokerAPI::get_funding_wallet(const std::string& account_id) {
    return BrokerEntity(httpClient.get("/v1beta/accounts/" + account_id + "/funding_wallet"));
}

BrokerEntity AlpacaBrokerAPI::create_funding_wallet(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1beta/accounts/" + account_id + "/funding_wallet", request));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::list_funding_details(const std::string& account_id, const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1beta/accounts/" + account_id + "/funding_wallet/funding_details", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_funding_wallet_transfers(const std::string& account_id, const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1beta/accounts/" + account_id + "/funding_wallet/transfers", query)));
}

BrokerEntity AlpacaBrokerAPI::get_funding_wallet_transfer_by_id(const std::string& account_id, const std::string& transfer_id) {
    return BrokerEntity(httpClient.get("/v1beta/accounts/" + account_id + "/funding_wallet/transfers/" + transfer_id));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_funding_wallet_recipient_bank(const std::string& account_id) {
    return parse_array<BrokerEntity>(httpClient.get("/v1beta/accounts/" + account_id + "/funding_wallet/recipient_bank"));
}

BrokerEntity AlpacaBrokerAPI::create_funding_wallet_recipient_bank(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1beta/accounts/" + account_id + "/funding_wallet/recipient_bank", request));
}

void AlpacaBrokerAPI::delete_funding_wallet_recipient_bank(const std::string& account_id) {
    httpClient.del("/v1beta/accounts/" + account_id + "/funding_wallet/recipient_bank");
}

BrokerEntity AlpacaBrokerAPI::create_funding_wallet_withdrawal(const std::string& account_id, const json& request) {
    return BrokerEntity(httpClient.post("/v1beta/accounts/" + account_id + "/funding_wallet/withdrawal", request));
}

// ---------------------------------------------------------------------------
// Rebalancing
// ---------------------------------------------------------------------------

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_rebalancing_portfolios(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/rebalancing/portfolios", query)));
}

BrokerEntity AlpacaBrokerAPI::post_v1_rebalancing_portfolios(const json& request) {
    return BrokerEntity(httpClient.post("/v1/rebalancing/portfolios", request));
}

BrokerEntity AlpacaBrokerAPI::get_v1_rebalancing_portfolio(const std::string& portfolio_id) {
    return BrokerEntity(httpClient.get("/v1/rebalancing/portfolios/" + portfolio_id));
}

BrokerEntity AlpacaBrokerAPI::patch_v1_rebalancing_portfolio(const std::string& portfolio_id, const json& request) {
    return BrokerEntity(httpClient.patch("/v1/rebalancing/portfolios/" + portfolio_id, request));
}

void AlpacaBrokerAPI::delete_v1_rebalancing_portfolio(const std::string& portfolio_id) {
    httpClient.del("/v1/rebalancing/portfolios/" + portfolio_id);
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_rebalancing_subscriptions(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/rebalancing/subscriptions", query)));
}

BrokerEntity AlpacaBrokerAPI::post_v1_rebalancing_subscriptions(const json& request) {
    return BrokerEntity(httpClient.post("/v1/rebalancing/subscriptions", request));
}

BrokerEntity AlpacaBrokerAPI::get_v1_rebalancing_subscription(const std::string& subscription_id) {
    return BrokerEntity(httpClient.get("/v1/rebalancing/subscriptions/" + subscription_id));
}

void AlpacaBrokerAPI::delete_v1_rebalancing_subscription(const std::string& subscription_id) {
    httpClient.del("/v1/rebalancing/subscriptions/" + subscription_id);
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_rebalancing_runs(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/rebalancing/runs", query)));
}

BrokerEntity AlpacaBrokerAPI::post_v1_rebalancing_runs(const json& request) {
    return BrokerEntity(httpClient.post("/v1/rebalancing/runs", request));
}

BrokerEntity AlpacaBrokerAPI::get_v1_rebalancing_run(const std::string& run_id) {
    return BrokerEntity(httpClient.get("/v1/rebalancing/runs/" + run_id));
}

void AlpacaBrokerAPI::delete_v1_rebalancing_run(const std::string& run_id) {
    httpClient.del("/v1/rebalancing/runs/" + run_id);
}

// ---------------------------------------------------------------------------
// Reporting / Country / IRA
// ---------------------------------------------------------------------------

BrokerEntity AlpacaBrokerAPI::get_v1beta1_logo(const std::string& symbol) {
    return BrokerEntity(httpClient.get("/v1beta1/logos/" + symbol));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_reporting_eod_aggregate_positions(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/reporting/eod/aggregate_positions", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_reporting_eod_positions(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/reporting/eod/positions", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::get_v1_reporting_eod_cash_interest(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/reporting/eod/cash_interest", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::query_country_infos(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/country-info", query)));
}

std::vector<BrokerEntity> AlpacaBrokerAPI::list_ira_excess_contributions(const std::string& query) {
    return parse_array<BrokerEntity>(httpClient.get(with_query("/v1/accounts/ira_excess_contributions", query)));
}

// ---------------------------------------------------------------------------
// Crypto Wallets (Account-scoped)
// ---------------------------------------------------------------------------

std::vector<CryptoWallet> AlpacaBrokerAPI::list_crypto_funding_wallets_for_account(const std::string& account_id) {
    return parse_array<CryptoWallet>(httpClient.get("/v1/accounts/" + account_id + "/wallets"));
}

std::vector<CryptoTransfer> AlpacaBrokerAPI::list_crypto_funding_transfers_for_account(const std::string& account_id, const std::string& asset) {
    auto query = QueryBuilder("/v1/accounts/" + account_id + "/wallets/transfers")
        .add("asset", asset)
        .build();

    return parse_array<CryptoTransfer>(httpClient.get(query));
}

CryptoTransfer AlpacaBrokerAPI::create_crypto_transfer_for_account(const std::string& account_id, const CryptoTransferRequest& request) {
    return CryptoTransfer(httpClient.post("/v1/accounts/" + account_id + "/wallets/transfers", request.to_json()));
}

CryptoTransfer AlpacaBrokerAPI::get_crypto_funding_transfer_for_account(const std::string& account_id, const std::string& transfer_id) {
    return CryptoTransfer(httpClient.get("/v1/accounts/" + account_id + "/wallets/transfers/" + transfer_id));
}

std::vector<WhitelistedAddress> AlpacaBrokerAPI::list_whitelisted_addresses_for_account(const std::string& account_id) {
    return parse_array<WhitelistedAddress>(httpClient.get("/v1/accounts/" + account_id + "/wallets/whitelists"));
}

WhitelistedAddress AlpacaBrokerAPI::create_whitelisted_address_for_account(const std::string& account_id, const WhitelistedAddressRequest& request) {
    return WhitelistedAddress(httpClient.post("/v1/accounts/" + account_id + "/wallets/whitelists", request.to_json()));
}

void AlpacaBrokerAPI::delete_whitelisted_address_for_account(const std::string& account_id, const std::string& whitelisted_address_id) {
    httpClient.del("/v1/accounts/" + account_id + "/wallets/whitelists/" + whitelisted_address_id);
}

CryptoTransferEstimate AlpacaBrokerAPI::get_crypto_transfer_estimate(const std::string& asset, const std::string& network, const Decimal& amount) {
    auto query = QueryBuilder("/v1/wallets/fees/estimate")
        .add("asset", asset)
        .add("network", network)
        .add("amount", amount)
        .build();

    return CryptoTransferEstimate(httpClient.get(query));
}

} // namespace alpaca
