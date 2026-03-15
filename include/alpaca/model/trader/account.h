#pragma once

#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/core/util.h"
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
    Decimal buying_power;
    Decimal regt_buying_power;
    Decimal daytrading_buying_power;
    Decimal effective_buying_power;
    Decimal non_marginable_buying_power;
    Decimal options_buying_power;
    Decimal bod_dtbp;
    Decimal cash;
    Decimal accrued_fees;
    Decimal portfolio_value;
    bool pattern_day_trader;
    bool trading_blocked;
    bool transfers_blocked;
    bool account_blocked;
    DateTime created_at;
    bool trade_suspended_by_user;
    std::string multiplier;
    bool shorting_enabled;
    Decimal equity;
    Decimal last_equity;
    Decimal long_market_value;
    Decimal short_market_value;
    Decimal position_market_value;
    Decimal initial_margin;
    Decimal maintenance_margin;
    Decimal last_maintenance_margin;
    Decimal sma;
    int daytrade_count;
    Decimal balance_asof;
    int crypto_tier;
    Decimal intraday_adjustments;
    Decimal pending_reg_taf_fees;

    public:

    /**
     * Constructor to initialize the Account object from a JSON object.
     * @param j JSON object containing account information.
     */
    Account(const json& j)
    : 
        id(parse_string(j, "id")),
        account_number(parse_string(j, "account_number")),
        status(parse_string(j, "status")),
        crypto_status(parse_string(j, "crypto_status")),
        options_approved_level(parse_int(j, "options_approved_level")),
        options_trading_level(parse_int(j, "options_trading_level")),
        currency(parse_string(j, "currency")),
        buying_power(parse_decimal(j, "buying_power")),
        regt_buying_power(parse_decimal(j, "regt_buying_power")),
        daytrading_buying_power(parse_decimal(j, "daytrading_buying_power")),
        effective_buying_power(parse_decimal(j, "effective_buying_power")),
        non_marginable_buying_power(parse_decimal(j, "non_marginable_buying_power")),
        options_buying_power(parse_decimal(j, "options_buying_power")),
        bod_dtbp(parse_decimal(j, "bod_dtbp")),
        cash(parse_decimal(j, "cash")),
        accrued_fees(parse_decimal(j, "accrued_fees")),
        portfolio_value(parse_decimal(j, "portfolio_value")),
        pattern_day_trader(parse_bool(j, "pattern_day_trader")),
        trading_blocked(parse_bool(j, "trading_blocked")),
        transfers_blocked(parse_bool(j, "transfers_blocked")),
        account_blocked(parse_bool(j, "account_blocked")),
        created_at(parse_datetime(j, "created_at")),
        trade_suspended_by_user(parse_bool(j, "trade_suspended_by_user")),
        multiplier(parse_string(j, "multiplier")),
        shorting_enabled(parse_bool(j, "shorting_enabled")),
        equity(parse_decimal(j, "equity")),
        last_equity(parse_decimal(j, "last_equity")),
        long_market_value(parse_decimal(j, "long_market_value")),
        short_market_value(parse_decimal(j, "short_market_value")),
        position_market_value(parse_decimal(j, "position_market_value")),
        initial_margin(parse_decimal(j, "initial_margin")),
        maintenance_margin(parse_decimal(j, "maintenance_margin")),
        last_maintenance_margin(parse_decimal(j, "last_maintenance_margin")),
        sma(parse_decimal(j, "sma")),
        daytrade_count(parse_int(j, "daytrade_count")),
        balance_asof(parse_decimal(j, "balance_asof")),
        crypto_tier(parse_int(j, "crypto_tier")),
        intraday_adjustments(parse_decimal(j, "intraday_adjustments")),
        pending_reg_taf_fees(parse_decimal(j, "pending_reg_taf_fees"))
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
        j["buying_power"] = buying_power.to_double();
        j["regt_buying_power"] = regt_buying_power.to_double();
        j["daytrading_buying_power"] = daytrading_buying_power.to_double();
        j["effective_buying_power"] = effective_buying_power.to_double();
        j["non_marginable_buying_power"] = non_marginable_buying_power.to_double();
        j["options_buying_power"] = options_buying_power.to_double();
        j["bod_dtbp"] = bod_dtbp.to_double();
        j["cash"] = cash.to_double();
        j["accrued_fees"] = accrued_fees.to_double();
        j["portfolio_value"] = portfolio_value.to_double();
        j["pattern_day_trader"] = pattern_day_trader;
        j["trading_blocked"] = trading_blocked;
        j["transfers_blocked"] = transfers_blocked;
        j["account_blocked"] = account_blocked;
        j["created_at"] = created_at.to_string();
        j["trade_suspended_by_user"] = trade_suspended_by_user;
        j["multiplier"] = multiplier;
        j["shorting_enabled"] = shorting_enabled;
        j["equity"] = equity.to_double();
        j["last_equity"] = last_equity.to_double();
        j["long_market_value"] = long_market_value.to_double();
        j["short_market_value"] = short_market_value.to_double();
        j["position_market_value"] = position_market_value.to_double();
        j["initial_margin"] = initial_margin.to_double();
        j["maintenance_margin"] = maintenance_margin.to_double();
        j["last_maintenance_margin"] = last_maintenance_margin.to_double();
        j["sma"] = sma.to_double();
        j["daytrade_count"] = daytrade_count;
        j["balance_asof"] = balance_asof.to_double();
        j["crypto_tier"] = crypto_tier;
        j["intraday_adjustments"] = intraday_adjustments.to_double();
        j["pending_reg_taf_fees"] = pending_reg_taf_fees.to_double();
    
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
        dtbp_check = parse_string(j, "dtbp_check");
        trade_confirm_email = parse_string(j, "trade_confirm_email");
        suspend_trade = parse_bool(j, "suspend_trade");
        no_shorting = parse_bool(j, "no_shorting");
        fractional_trading = parse_bool(j, "fractional_trading");
        max_margin_multiplier = parse_int(j, "max_margin_multiplier");
        max_options_trading_level = parse_int(j, "max_options_trading_level");
        pdt_check = parse_string(j, "pdt_check");
        ptp_no_exception_entry = parse_bool(j, "ptp_no_exception_entry");
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