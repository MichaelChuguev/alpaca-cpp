#include "alpaca/api/rest/trader/AlpacaTraderAPI.h"
#include "alpaca/core/error.h"

namespace alpaca {

Account AlpacaTraderAPI::get_account() {
    try {
        return Account(httpClient.get("/v2/account"));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

AccountConfig AlpacaTraderAPI::send_account_config(const AccountConfig& account_config) {
    try {
        return AccountConfig(httpClient.patch("/v2/account/configurations", account_config.to_json()));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<Asset> AlpacaTraderAPI::get_assets(const std::string& status, const std::string& asset_class, const std::string& exchange, const std::vector<std::string>& attributes) {
    try {
        std::string query = "/v2/assets?";
        if (!status.empty()) query += "status=" + status + "&";
        if (!asset_class.empty()) query += "asset_class=" + asset_class + "&";
        if (!exchange.empty()) query += "exchange=" + exchange + "&";
        
        if (!attributes.empty()) {
            query += "attributes=";
            for (size_t i = 0; i < attributes.size(); ++i) {
                query += attributes[i];
                if (i < attributes.size() - 1) query += ",";
            }
            query += "&";
        }
    
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        
        json j = httpClient.get(query);
        std::vector<Asset> assets;
        for (const auto& asset : j) {
            assets.push_back(Asset(asset));
        }
        return assets;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Asset AlpacaTraderAPI::get_asset(const std::string& symbol_id) {
    try {
        return Asset(httpClient.get("/v2/assets/" + symbol_id));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<OptionContract> AlpacaTraderAPI::get_option_contracts(const std::vector<std::string>& underlying_symbols, const bool& show_deliverables, const OptionContractStatus& status, const DateTime& expiration_date, const DateTime& expiration_date_gte, const DateTime& expiration_date_lte, const std::string& root_symbol, const OptionContractType& type, const OptionContractStyle& style, const double& strike_price_gte, const double& strike_price_lte, const std::string& page_token, const int& limit, const bool& ppind) {
    try {
        std::string query = "/v2/options/contracts?";
        if (!underlying_symbols.empty()) {
            query += "underlying_symbols=";
            for (size_t i = 0; i < underlying_symbols.size(); ++i) {
                query += underlying_symbols[i];
                if (i < underlying_symbols.size() - 1) query += ",";
            }
            query += "&";
        }
        if (show_deliverables) query += "show_deliverables=true&";
        if (status == OptionContractStatus::ACTIVE) query += "status=active&";
        else if (status == OptionContractStatus::INACTIVE) query += "status=inactive&";
        if (expiration_date.is_date_only()) query += "expiration_date=" + expiration_date.to_iso_string() + "&";
        if (expiration_date_gte.is_date_only()) query += "expiration_date_gte=" + expiration_date_gte.to_iso_string() + "&";
        if (expiration_date_lte.is_date_only()) query += "expiration_date_lte=" + expiration_date_lte.to_iso_string() + "&";
        if (!root_symbol.empty()) query += "root_symbol=" + root_symbol + "&";
        if (type == OptionContractType::CALL) query += "type=call&";
        else if (type == OptionContractType::PUT) query += "type=put&";
        if (style == OptionContractStyle::AMERICAN) query += "style=american&";
        else if (style == OptionContractStyle::EUROPEAN) query += "style=european&";
        if (strike_price_gte > 0) query += "strike_price_gte=" + std::to_string(strike_price_gte) + "&";
        if (strike_price_lte > 0) query += "strike_price_lte=" + std::to_string(strike_price_lte) + "&";
        if (!page_token.empty()) query += "page_token=" + page_token + "&";
        if (limit > 0) query += "limit=" + std::to_string(limit) + "&";
        if (ppind) query += "ppind=true&";

        if (query.back() == '&' || query.back() == '?') query.pop_back();

        std::vector<OptionContract> all_contracts;
        std::string next_page_token = page_token;

        do {
            std::string paginated_query = query;
            if (!next_page_token.empty()) {
                paginated_query += "&page_token=" + next_page_token;
            }

            json j = httpClient.get(paginated_query);
            for (const auto& contract : j["contracts"]) {
                all_contracts.push_back(OptionContract(contract));
            }

            if (j.contains("next_page_token")) {
                next_page_token = j["next_page_token"].get<std::string>();
            } else {
                next_page_token.clear();
            }
        } while (!next_page_token.empty());

        return all_contracts;

    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

OptionContract AlpacaTraderAPI::get_option_contract(const std::string& symbol_id) {
    try {
        return OptionContract(httpClient.get("/v2/options/contracts/" + symbol_id));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<Treasury> AlpacaTraderAPI::get_us_treasuries(const Subtype& subtype, const BondStatus& bond_status, const std::vector<std::string>& cusips, const std::vector<std::string>& isins) {
    try {
        std::string query = "/v2/assets/fixed_income/us_treasuries?";
        if (subtype == Subtype::BOND) query += "subtype=bond&";
        else if (subtype == Subtype::BILL) query += "subtype=bill&";
        else if (subtype == Subtype::NOTE) query += "subtype=note&";
        else if (subtype == Subtype::STRIPS) query += "subtype=strips&";
        else if (subtype == Subtype::TIPS) query += "subtype=tips&";
        else if (subtype == Subtype::FLOATING_RATE) query += "subtype=floating_rate&";

        if (bond_status == BondStatus::OUTSTANDING) query += "bond_status=outstanding&";
        else if (bond_status == BondStatus::MATURED) query += "bond_status=matured&";
        else if (bond_status == BondStatus::PRE_ISSUANCE) query += "bond_status=pre_issuance&";

        if (!cusips.empty()) {
            query += "cusips=";
            for (size_t i = 0; i < cusips.size(); ++i) {
                query += cusips[i];
                if (i < cusips.size() - 1) query += ",";
            }
            query += "&";
        }

        if (!isins.empty()) {
            query += "isins=";
            for (size_t i = 0; i < isins.size(); ++i) {
                query += isins[i];
                if (i < isins.size() - 1) query += ",";
            }
            query += "&";
        }

        if (query.back() == '&' || query.back() == '?') query.pop_back();

        json j = httpClient.get(query);
        std::vector<Treasury> treasuries;
        for (const auto& treasury : j["treasuries"]) {
            treasuries.push_back(Treasury(treasury));
        }
        return treasuries;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

OrderResponse AlpacaTraderAPI::create_order(const Order& order) {
    try {
        return OrderResponse(httpClient.post("/v2/orders", order.to_json()));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<OrderResponse> AlpacaTraderAPI::get_orders(const OrderStatus& status, const int& limit, const DateTime& after, const DateTime& until, const Sort& direction, const bool& nested, const std::vector<std::string>& symbols, const OrderSide& side) {
    try {
        std::string query = "/v2/orders?";
        if (status == OrderStatus::OPEN) query += "status=open&";
        else if (status == OrderStatus::CLOSED) query += "status=closed&";
        else if (status == OrderStatus::ALL) query += "status=all&";

        if (limit > 0) query += "limit=" + std::to_string(limit) + "&";
        if (after.is_date_only()) query += "after=" + after.to_iso_string() + "&";
        if (until.is_date_only()) query += "until=" + until.to_iso_string() + "&";
        if (direction == Sort::ASC) query += "direction=asc&";
        else if (direction == Sort::DESC) query += "direction=desc&";
        if (nested) query += "nested=true&";

        if (!symbols.empty()) {
            query += "symbols=";
            for (size_t i = 0; i < symbols.size(); ++i) {
                query += symbols[i];
                if (i < symbols.size() - 1) query += ",";
            }
            query += "&";
        }

        if (query.back() == '&' || query.back() == '?') query.pop_back();

        json j = httpClient.get(query);
        std::vector<OrderResponse> orders;
        for (const auto& order : j["orders"]) {
            orders.push_back(OrderResponse(order));
        }
        return orders;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::map<std::string, int> AlpacaTraderAPI::delete_all_orders() {
    try {
        json j = httpClient.del("/v2/orders");
        std::map<std::string, int> response_map;
        for (const auto& order : j) {
            response_map[order["id"].get<std::string>()] = order["status"].get<int>();
        }
        return response_map;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

OrderResponse AlpacaTraderAPI::get_order_by_client_order_id(const std::string& client_order_id) {
    try {
        return OrderResponse(httpClient.get("/v2/orders/" + client_order_id));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

OrderResponse AlpacaTraderAPI::get_order_by_id(const std::string& order_id, const bool& nested) {
    try {
        std::string query = "/v2/orders/" + order_id;
        if (nested) query += "?nested=true";
        return OrderResponse(httpClient.get(query));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

OrderResponse AlpacaTraderAPI::replace_order(const std::string& order_id, const int& qty, const OrderTimeInForce& time_in_force, const double& limit_price, const double& stop_price, const double& trail, std::string& client_order_id) {
    try {
        json j;
        j["qty"] = qty;
        /*
        enum OrderTimeInForce {
        DAY, // Day Order
        GTC, // Good Till Cancelled
        OPG, 
        CLS, 
        IOC, // Immediate Or Cancel
        FOK, // Fill Or Kill 
    };
        */
        if (time_in_force == OrderTimeInForce::DAY) j["time_in_force"] = "day";
        else if (time_in_force == OrderTimeInForce::GTC) j["time_in_force"] = "gtc";
        else if (time_in_force == OrderTimeInForce::OPG) j["time_in_force"] = "opg";
        else if (time_in_force == OrderTimeInForce::CLS) j["time_in_force"] = "cls";
        else if (time_in_force == OrderTimeInForce::IOC) j["time_in_force"] = "ioc";
        else if (time_in_force == OrderTimeInForce::FOK) j["time_in_force"] = "fok";

        j["limit_price"] = limit_price;
        j["stop_price"] = stop_price;
        j["trail_price"] = trail;
        j["client_order_id"] = client_order_id;

        return OrderResponse(httpClient.patch("/v2/orders/" + order_id, j));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

void AlpacaTraderAPI::delete_order(const std::string& order_id) {
    try {
        httpClient.del("/v2/orders/" + order_id);
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

//getAllPositions
std::vector<Position> AlpacaTraderAPI::get_all_positions() {
    try {
        json j = httpClient.get("/v2/positions");
        std::vector<Position> positions;
        for (const auto& position : j) {
            positions.push_back(Position(position));
        }
        return positions;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;

    } 
}

std::vector<PositionClosed> AlpacaTraderAPI::close_all_positions(const bool& cancel_orders) {
    try {
        std::string query = "/v2/positions";
        if (cancel_orders) query += "?cancel_orders=true";
        json j = httpClient.del(query);
        std::vector<PositionClosed> closed;
        for (const auto& pos : j) {
            closed.push_back(PositionClosed(pos));
        }
        return closed;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Position AlpacaTraderAPI::get_open_position(const std::string& symbol_id) {
    try {
        return Position(httpClient.get("/v2/positions/" + symbol_id));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

OrderResponse AlpacaTraderAPI::close_position(const std::string& symbol_id, const double& qty, const double& percentage) {
    try {
        std::string query = "/v2/positions/" + symbol_id + "?";
        if (qty > 0) query += "qty=" + std::to_string(qty) + "&";
        if (percentage > 0) query += "percentage=" + std::to_string(percentage) + "&";
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        return OrderResponse(httpClient.del(query));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

void AlpacaTraderAPI::exercise_option(const std::string& symbol_contract_id) {
    try {
        httpClient.post("/v2/positions/" + symbol_contract_id + "/exercise", json::object());
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

PortfolioHistory AlpacaTraderAPI::get_portfolio_history(const std::string& period, const std::string& timeframe, const IntradayReporting& reporting, const DateTime& start, const PnlReset& reset, const DateTime& end, const std::string& cashflow_types) {
    try {
        std::string query = "/v2/account/portfolio/history?";
        if (!period.empty()) query += "period=" + period + "&";
        if (!timeframe.empty()) query += "timeframe=" + timeframe + "&";
        if (reporting == IntradayReporting::MARKET_HOURS) query += "intraday_reporting=market_hours&";
        else if (reporting == IntradayReporting::EXTENDED_HOURS) query += "intraday_reporting=extended_hours&";
        else if (reporting == IntradayReporting::CONTINUOUS) query += "intraday_reporting=continuous&";
        if (start.is_date_only()) query += "start=" + start.to_iso_string() + "&";
        if (end.is_date_only()) query += "end=" + end.to_iso_string() + "&";
        if (reset == PnlReset::PER_DAY) query += "pnl_reset=per_day&";
        else if (reset == PnlReset::NO_RESET) query += "pnl_reset=no_reset&";
        if (!cashflow_types.empty()) query += "cashflow_types=" + cashflow_types + "&";
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        return PortfolioHistory(httpClient.get(query));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<Watchlist> AlpacaTraderAPI::get_all_watchlists() {
    try {
        json j = httpClient.get("/v2/watchlists");
        std::vector<Watchlist> watchlists;
        for (const auto& wl : j) {
            watchlists.push_back(Watchlist(wl));
        }
        return watchlists;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::create_watchlist(const std::string& name, const std::vector<std::string>& symbols) {
    try {
        json body;
        body["name"] = name;
        body["symbols"] = symbols;
        return Watchlist(httpClient.post("/v2/watchlists", body));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::get_watchlist_by_id(const std::string& id) {
    try {
        return Watchlist(httpClient.get("/v2/watchlists/" + id));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::update_watchlist(const std::string& id, const std::string& name, const std::vector<std::string>& symbols) {
    try {
        json body;
        body["name"] = name;
        body["symbols"] = symbols;
        return Watchlist(httpClient.put("/v2/watchlists/" + id, body));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::add_asset_to_watchlist(const std::string& id, const std::string& symbol) {
    try {
        json body;
        body["symbol"] = symbol;
        return Watchlist(httpClient.post("/v2/watchlists/" + id, body));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

void AlpacaTraderAPI::delete_watchlist(const std::string& id) {
    try {
        httpClient.del("/v2/watchlists/" + id);
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::get_watchlist_by_name(const std::string& name) {
    try {
        return Watchlist(httpClient.get("/v2/watchlists:by_name?name=" + name));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::update_watchlist_by_name(const std::string& name, const std::vector<std::string>& symbols) {
    try {
        json body;
        body["name"] = name;
        body["symbols"] = symbols;
        return Watchlist(httpClient.put("/v2/watchlists:by_name", body));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::add_asset_to_watchlist_by_name(const std::string& name, const std::string& symbol) {
    try {
        json body;
        body["name"] = name;
        body["symbol"] = symbol;
        return Watchlist(httpClient.post("/v2/watchlists:by_name", body));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

void AlpacaTraderAPI::delete_watchlist_by_name(const std::string& name) {
    try {
        httpClient.del("/v2/watchlists:by_name?name=" + name);
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Watchlist AlpacaTraderAPI::remove_asset_from_watchlist(const std::string& id, const std::string& symbol) {
    try {
        return Watchlist(httpClient.del("/v2/watchlists/" + id + "/" + symbol));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

AccountConfig AlpacaTraderAPI::get_account_config() {
    try {
        return AccountConfig(httpClient.get("/v2/account/configurations"));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<AccountActivity> AlpacaTraderAPI::get_account_activities(const DateTime& after, const DateTime& until, const Sort& direction, const int& page_size, const std::string& page_token, const std::string& category) {
    try {
        std::string query = "/v2/account/activities?";
        if (after.is_date_only()) query += "after=" + after.to_iso_string() + "&";
        if (until.is_date_only()) query += "until=" + until.to_iso_string() + "&";
        if (direction == Sort::ASC) query += "direction=asc&";
        else if (direction == Sort::DESC) query += "direction=desc&";
        if (page_size > 0) query += "page_size=" + std::to_string(page_size) + "&";
        if (!page_token.empty()) query += "page_token=" + page_token + "&";
        if (!category.empty()) query += "category=" + category + "&";
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        json j = httpClient.get(query);
        std::vector<AccountActivity> activities;
        for (const auto& act : j) {
            activities.push_back(AccountActivity(act));
        }
        return activities;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<AccountActivity> AlpacaTraderAPI::get_account_activities_by_type(const std::string& activity_type, const DateTime& after, const DateTime& until, const Sort& direction, const int& page_size, const std::string& page_token) {
    try {
        std::string query = "/v2/account/activities/" + activity_type + "?";
        if (after.is_date_only()) query += "after=" + after.to_iso_string() + "&";
        if (until.is_date_only()) query += "until=" + until.to_iso_string() + "&";
        if (direction == Sort::ASC) query += "direction=asc&";
        else if (direction == Sort::DESC) query += "direction=desc&";
        if (page_size > 0) query += "page_size=" + std::to_string(page_size) + "&";
        if (!page_token.empty()) query += "page_token=" + page_token + "&";
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        json j = httpClient.get(query);
        std::vector<AccountActivity> activities;
        for (const auto& act : j) {
            activities.push_back(AccountActivity(act));
        }
        return activities;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Calendar AlpacaTraderAPI::get_calendar(const DateTime& start, const DateTime& end) {
    try {
        std::string query = "/v2/calendar?";
        if (start.is_date_only()) query += "start=" + start.to_iso_string() + "&";
        if (end.is_date_only()) query += "end=" + end.to_iso_string() + "&";
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        return Calendar(httpClient.get(query));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Clock AlpacaTraderAPI::get_clock() {
    try {
        return Clock(httpClient.get("/v2/clock"));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Calendar AlpacaTraderAPI::get_calendar_v3(const std::string& market, const DateTime& start, const DateTime& end) {
    try {
        std::string query = "/v3/calendar/" + market + "?";
        if (start.is_date_only()) query += "start=" + start.to_iso_string() + "&";
        if (end.is_date_only()) query += "end=" + end.to_iso_string() + "&";
        if (query.back() == '&' || query.back() == '?') query.pop_back();
        return Calendar(httpClient.get(query));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

Clock AlpacaTraderAPI::get_clock_v3() {
    try {
        return Clock(httpClient.get("/v3/clock"));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<CryptoWallet> AlpacaTraderAPI::get_crypto_wallets() {
    try {
        json j = httpClient.get("/v2/wallets");
        std::vector<CryptoWallet> wallets;
        for (const auto& w : j) {
            wallets.push_back(CryptoWallet(w));
        }
        return wallets;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<CryptoTransfer> AlpacaTraderAPI::get_crypto_transfers(const std::string& asset) {
    try {
        std::string query = "/v2/wallets/transfers";
        if (!asset.empty()) query += "?asset=" + asset;
        json j = httpClient.get(query);
        std::vector<CryptoTransfer> transfers;
        for (const auto& t : j) {
            transfers.push_back(CryptoTransfer(t));
        }
        return transfers;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

CryptoTransfer AlpacaTraderAPI::create_crypto_transfer(const CryptoTransferRequest& request) {
    try {
        return CryptoTransfer(httpClient.post("/v2/wallets/transfers", request.to_json()));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

CryptoTransfer AlpacaTraderAPI::get_crypto_transfer(const std::string& transfer_id) {
    try {
        return CryptoTransfer(httpClient.get("/v2/wallets/transfers/" + transfer_id));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

std::vector<WhitelistedAddress> AlpacaTraderAPI::get_whitelisted_addresses() {
    try {
        json j = httpClient.get("/v2/wallets/whitelists");
        std::vector<WhitelistedAddress> addresses;
        for (const auto& a : j) {
            addresses.push_back(WhitelistedAddress(a));
        }
        return addresses;
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

WhitelistedAddress AlpacaTraderAPI::create_whitelisted_address(const WhitelistedAddressRequest& request) {
    try {
        return WhitelistedAddress(httpClient.post("/v2/wallets/whitelists", request.to_json()));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

void AlpacaTraderAPI::delete_whitelisted_address(const std::string& whitelisted_address_id) {
    try {
        httpClient.del("/v2/wallets/whitelists/" + whitelisted_address_id);
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

CryptoTransferEstimate AlpacaTraderAPI::get_crypto_transfer_estimate(const std::string& asset, const std::string& network, const double& amount) {
    try {
        std::string query = "/v2/wallets/fees/estimate?asset=" + asset;
        if (!network.empty()) query += "&network=" + network;
        if (amount > 0) query += "&amount=" + std::to_string(amount);
        return CryptoTransferEstimate(httpClient.get(query));
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

void AlpacaTraderAPI::do_not_exercise_option(const std::string& symbol_contract_id) {
    try {
        httpClient.post("/v2/positions/" + symbol_contract_id + "/do-not-exercise", json::object());
    } catch (const AlpacaTraderException& e) {
        std::cerr << "Error: " << e.what() << " (code: " << e.code() << ")" << std::endl;
    }
}

} // namespace alpaca