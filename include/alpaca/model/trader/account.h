#pragma once

#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/api/rest/HTTPClient.h"

namespace alpaca {

using json = nlohmann::json;

struct Account {

    std::string id;
    std::string account_number;
    std::string status;
    std::string crypto_status;
    int options_approved_level;
    int options_trading_level;
    std::string currency;
    double buying_power;
    double regt_buying_power;
    double daytrading_buying_power;
    double effective_buying_power;
    double non_marginable_buying_power;
    double options_buying_power;
    double bod_dtbp;
    double cash;
    double accrued_fees;
    double portfolio_value;
    bool pattern_day_trader;
    bool trading_blocked;
    bool transfers_blocked;
    bool account_blocked;
    DateTime created_at;
    bool trade_suspended_by_user;
    std::string multiplier;
    bool shorting_enabled;
    double equity;
    double last_equity;
    double long_market_value;
    double short_market_value;
    double position_market_value;
    double initial_margin;
    double maintenance_margin;
    double last_maintenance_margin;
    double sma;
    int daytrade_count;
    double balance_asof;
    int crypto_tier;
    double intraday_adjustments;
    double pending_reg_taf_fees;

    public:

    /**
     * Constructor to initialize the Account object from a JSON object.
     * @param j JSON object containing account information.
     */
    Account(const json& j)
    : 
        id(j["id"].get<std::string>()),
        account_number(j["account_number"].get<std::string>()),
        status(j["status"].get<std::string>()),
        crypto_status(j["crypto_status"].get<std::string>()),
        options_approved_level(j["options_approved_level"].get<int>()),
        options_trading_level(j["options_trading_level"].get<int>()),
        currency(j["currency"].get<std::string>()),
        buying_power(std::stod(j["buying_power"].get<std::string>())),
        regt_buying_power(std::stod(j["regt_buying_power"].get<std::string>())),
        daytrading_buying_power(std::stod(j["daytrading_buying_power"].get<std::string>())),
        effective_buying_power(std::stod(j["effective_buying_power"].get<std::string>())),
        non_marginable_buying_power(std::stod(j["non_marginable_buying_power"].get<std::string>())),
        options_buying_power(std::stod(j["options_buying_power"].get<std::string>())),
        bod_dtbp(std::stod(j["bod_dtbp"].get<std::string>())),
        cash(std::stod(j["cash"].get<std::string>())),
        accrued_fees(std::stod(j["accrued_fees"].get<std::string>())),
        portfolio_value(std::stod(j["portfolio_value"].get<std::string>())),
        pattern_day_trader(j["pattern_day_trader"].get<bool>()),
        trading_blocked(j["trading_blocked"].get<bool>()),
        transfers_blocked(j["transfers_blocked"].get<bool>()),
        account_blocked(j["account_blocked"].get<bool>()),
        created_at(DateTime::parse(j["created_at"])),
        trade_suspended_by_user(j["trade_suspended_by_user"].get<bool>()),
        multiplier(j["multiplier"].get<std::string>()),
        shorting_enabled(j["shorting_enabled"].get<bool>()),
        equity(std::stod(j["equity"].get<std::string>())),
        last_equity(std::stod(j["last_equity"].get<std::string>())),
        long_market_value(std::stod(j["long_market_value"].get<std::string>())),
        short_market_value(std::stod(j["short_market_value"].get<std::string>())),
        position_market_value(std::stod(j["position_market_value"].get<std::string>())),
        initial_margin(std::stod(j["initial_margin"].get<std::string>())),
        maintenance_margin(std::stod(j["maintenance_margin"].get<std::string>())),
        last_maintenance_margin(std::stod(j["last_maintenance_margin"].get<std::string>())),
        sma(std::stod(j["sma"].get<std::string>())),
        daytrade_count(j["daytrade_count"].get<int>()),
        balance_asof(std::stod(j["balance_asof"].get<std::string>())),
        crypto_tier(j["crypto_tier"].get<int>()),
        intraday_adjustments(std::stod(j["intraday_adjustments"].get<std::string>())),
        pending_reg_taf_fees(std::stod(j["pending_reg_taf_fees"].get<std::string>()))
    {
    }


    std::string to_string() const {
        std::ostringstream oss;
        oss << "Account Information:\n"
            << "ID: " << id << "\n"
            << "Account Number: " << account_number << "\n"
            << "Status: " << status << "\n"
            << "Crypto Status: " << crypto_status << "\n"
            << "Options Approved Level: " << options_approved_level << "\n"
            << "Options Trading Level: " << options_trading_level << "\n"
            << "Currency: " << currency << "\n"
            << "Buying Power: " << buying_power << "\n"
            << "REGT Buying Power: " << regt_buying_power << "\n"
            << "Daytrading Buying Power: " << daytrading_buying_power << "\n"
            << "Effective Buying Power: " << effective_buying_power << "\n"
            << "Non-marginable Buying Power: " << non_marginable_buying_power << "\n"
            << "Options Buying Power: " << options_buying_power << "\n"
            << "BOD DTBP: " << bod_dtbp << "\n"
            << "Cash: " << cash << "\n"
            << "Accrued Fees: " << accrued_fees << "\n"
            << "Portfolio Value: " << portfolio_value << "\n"
            << "Pattern Day Trader: " << (pattern_day_trader ? "True" : "False") << "\n"
            << "Trading Blocked: " << (trading_blocked ? "True" : "False") << "\n"
            << "Transfers Blocked: " << (transfers_blocked ? "True" : "False") << "\n"
            << "Account Blocked: " << (account_blocked ? "True" : "False") << "\n"
            << "Created At: " << created_at.to_string() << "\n"
            << "Trade Suspended By User: " << (trade_suspended_by_user ? "True" : "False") << "\n"
            << "Multiplier: " << multiplier << "\n"
            << "Shorting Enabled: " << (shorting_enabled ? "True" : "False") << "\n"
            << "Equity: " << equity << "\n"
            << "Last Equity: " << last_equity << "\n"
            << "Long Market Value: " << long_market_value << "\n"
            << "Short Market Value: " << short_market_value << "\n"
            << "Position Market Value: " << position_market_value << "\n"
            << "Initial Margin: " << initial_margin << "\n"
            << "Maintenance Margin: " << maintenance_margin << "\n"
            << "Last Maintenance Margin: " << last_maintenance_margin << "\n"
            << "SMA: " << sma << "\n"
            << "Daytrade Count: " << daytrade_count << "\n"
            << "Balance As Of: " << balance_asof << "\n"
            << "Crypto Tier: " << crypto_tier << "\n"
            << "Intraday Adjustments: " << intraday_adjustments << "\n"
            << "Pending REG TAF Fees: " << pending_reg_taf_fees << "\n";
    
        return oss.str();
    }

    /**
     * 
     */
    json to_json() const {
        json j;
        j["id"] = id;
        j["account_number"] = account_number;
        j["status"] = status;
        j["crypto_status"] = crypto_status;
        j["options_approved_level"] = options_approved_level;
        j["options_trading_level"] = options_trading_level;
        j["currency"] = currency;
        j["buying_power"] = buying_power;
        j["regt_buying_power"] = regt_buying_power;
        j["daytrading_buying_power"] = daytrading_buying_power;
        j["effective_buying_power"] = effective_buying_power;
        j["non_marginable_buying_power"] = non_marginable_buying_power;
        j["options_buying_power"] = options_buying_power;
        j["bod_dtbp"] = bod_dtbp;
        j["cash"] = cash;
        j["accrued_fees"] = accrued_fees;
        j["portfolio_value"] = portfolio_value;
        j["pattern_day_trader"] = pattern_day_trader;
        j["trading_blocked"] = trading_blocked;
        j["transfers_blocked"] = transfers_blocked;
        j["account_blocked"] = account_blocked;
        j["created_at"] = created_at.to_string();
        j["trade_suspended_by_user"] = trade_suspended_by_user;
        j["multiplier"] = multiplier;
        j["shorting_enabled"] = shorting_enabled;
        j["equity"] = equity;
        j["last_equity"] = last_equity;
        j["long_market_value"] = long_market_value;
        j["short_market_value"] = short_market_value;
        j["position_market_value"] = position_market_value;
        j["initial_margin"] = initial_margin;
        j["maintenance_margin"] = maintenance_margin;
        j["last_maintenance_margin"] = last_maintenance_margin;
        j["sma"] = sma;
        j["daytrade_count"] = daytrade_count;
        j["balance_asof"] = balance_asof;
        j["crypto_tier"] = crypto_tier;
        j["intraday_adjustments"] = intraday_adjustments;
        j["pending_reg_taf_fees"] = pending_reg_taf_fees;
    
        return j;
    }
    
};

struct AccountConfig {

    std::string dtbp_check;
    std::string trade_confirm_email;
    bool suspend_trade;
    bool no_shorting;
    bool fractional_trading;
    int max_margin_multiplier;
    int max_options_trading_level;
    std::string pdt_check;
    bool ptp_no_exception_entry;

    AccountConfig() = default;

    AccountConfig(const std::string& dtbp_check, const std::string& trade_confirm_email, const bool& suspend_trade, const bool& no_shorting,
        const bool& fractional_trading, const int& max_margin_multiplier, const int& max_options_trading_level,
        const std::string& pdt_check, const bool& ptp_no_exception_entry)
    :
        dtbp_check(dtbp_check),
        trade_confirm_email(trade_confirm_email),
        suspend_trade(suspend_trade),
        no_shorting(no_shorting),
        fractional_trading(fractional_trading),
        max_margin_multiplier(max_margin_multiplier),
        max_options_trading_level(max_options_trading_level),
        pdt_check(pdt_check),
        ptp_no_exception_entry(ptp_no_exception_entry)
    {
    }

    AccountConfig(const json& j) {
        dtbp_check = j["dtbp_check"].get<std::string>();
        trade_confirm_email = j["trade_confirm_email"].get<std::string>();
        suspend_trade = j["suspend_trade"].get<bool>();
        no_shorting = j["no_shorting"].get<bool>();
        fractional_trading = j["fractional_trading"].get<bool>();
        max_margin_multiplier = std::stoi(j["max_margin_multiplier"].get<std::string>());
        max_options_trading_level = j["max_options_trading_level"].get<int>();
        pdt_check = j["pdt_check"].get<std::string>();
        ptp_no_exception_entry = j["ptp_no_exception_entry"].get<bool>();
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Account Configuration:\n"
            << "DTBP Check: " << dtbp_check << "\n"
            << "Trade Confirm Email: " << trade_confirm_email << "\n"
            << "Suspend Trade: " << (suspend_trade ? "True" : "False") << "\n"
            << "No Shorting: " << (no_shorting ? "True" : "False") << "\n"
            << "Fractional Trading: " << (fractional_trading ? "True" : "False") << "\n"
            << "Max Margin Multiplier: " << max_margin_multiplier << "\n"
            << "Max Options Trading Level: " << max_options_trading_level << "\n"
            << "PDT Check: " << pdt_check << "\n"
            << "PTP No Exception Entry: " << (ptp_no_exception_entry ? "True" : "False") << "\n";
    
        return oss.str();
    }

    json to_json() const {
        validate();
        json j;
        j["dtbp_check"] = dtbp_check;
        j["trade_confirm_email"] = trade_confirm_email;
        j["suspend_trade"] = suspend_trade;
        j["no_shorting"] = no_shorting;
        j["fractional_trading"] = fractional_trading;
        j["max_margin_multiplier"] = std::to_string(max_margin_multiplier);
        j["max_options_trading_level"] = max_options_trading_level;
        j["pdt_check"] = pdt_check;
        j["ptp_no_exception_entry"] = ptp_no_exception_entry;
    
        return j;
    }

    private:

    /**
     * Validates the account configuration parameters to make sure they match API rules prior to being sent.
     * 
     * @note 1. dtbp_check - both, entry, or exit. Controls Day Trading Margin Call (DTMC) checks.
     * 
     * @note 2. all or none. If none, emails for order fills are not sent.
     * 
     * @note 3. suspend_trade - If true, new orders are blocked.
     * 
     * @note 4. no_shorting - If true, account becomes long-only mode.
     * 
     * @note 5. fractional_trading - If true, account is able to participate in fractional trading.
     * 
     * @note 6. max_margin_multiplier - Can be 1, 2, or 4.
     * 
     * @note 7. max_options_trading_level - The desired maximum options trading level. 0=disabled, 1=Covered Call/Cash-Secured Put, 
     *                                      2=Long Call/Put, 3=Spreads/Straddles.
     * 
     * @note 8. pdt_check - both, entry, or exit. If entry orders will be rejected on entering a position if it could result in PDT 
     *                      being set for the account. exit will reject exiting orders if they would result in PDT being set.
     * 
     * @note 9. ptp_no_exception_entry - If set to true then Alpaca will accept orders for PTP symbols with no exception. Default is false.
     * 
     */
    void validate() const {
    
        if (dtbp_check != "both" && dtbp_check != "entry" && dtbp_check != "exit") {
            throw std::invalid_argument("dtbp_check must be 'both', 'entry', or 'exit'");
        }
    
        if (trade_confirm_email != "all" && trade_confirm_email != "none") {
            throw std::invalid_argument("trade_confirm_email must be 'all' or 'none'");
        }
    
        if (max_margin_multiplier != 1 && max_margin_multiplier != 2 && max_margin_multiplier != 4) {
            throw std::invalid_argument("max_margin_multiplier must be 1, 2, or 4");
        }
    
        // Validate max_options_trading_level - must be between 0 and 3
        if (max_options_trading_level < 0 || max_options_trading_level > 3) {
            throw std::invalid_argument("max_options_trading_level must be between 0 and 3");
        }
    
        // Validate pdt_check - must be "both", "entry", or "exit"
        if (pdt_check != "both" && pdt_check != "entry" && pdt_check != "exit") {
            throw std::invalid_argument("pdt_check must be 'both', 'entry', or 'exit'");
        }
        
    }

};

} // namespace alpaca