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

OrderResponse AlpacaTraderAPI::get_order_by_id(const std::string& order_id, const bool& nested = false) {
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



} // namespace alpaca