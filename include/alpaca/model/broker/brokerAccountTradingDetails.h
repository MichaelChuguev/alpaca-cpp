#pragma once

#include <sstream>
#include <string>

#include "nlohmann/json.hpp"
#include "alpaca/core/util.h"
#include "alpaca/model/broker/brokerAccount.h"

namespace alpaca {

using json = nlohmann::json;

struct BrokerAccountTradingDetailsValueChange {
    std::string from;
    std::string to;

    BrokerAccountTradingDetailsValueChange() = default;
    BrokerAccountTradingDetailsValueChange(const json& j)
        : from(parse_string(j, "from")),
          to(parse_string(j, "to")) {
        if (from.empty()) from = parse_string(j, "old");
        if (to.empty()) to = parse_string(j, "new");
    }

    bool empty() const {
        return from.empty() && to.empty();
    }

    json to_json() const {
        json j = json::object();
        if (!from.empty()) j["from"] = from;
        if (!to.empty()) j["to"] = to;
        return j;
    }
};

struct BrokerRestrictToLiquidationReasons {
    bool pattern_day_trading = false;
    bool ach_return = false;
    bool position_to_equity_ratio = false;
    bool unspecified = false;

    BrokerRestrictToLiquidationReasons() = default;
    BrokerRestrictToLiquidationReasons(const json& j)
        : pattern_day_trading(parse_bool(j, "pattern_day_trading")),
          ach_return(parse_bool(j, "ach_return")),
          position_to_equity_ratio(parse_bool(j, "position_to_equity_ratio")),
          unspecified(parse_bool(j, "unspecified")) {}

    json to_json() const {
        json j;
        j["pattern_day_trading"] = pattern_day_trading;
        j["ach_return"] = ach_return;
        j["position_to_equity_ratio"] = position_to_equity_ratio;
        j["unspecified"] = unspecified;
        return j;
    }
};

struct BrokerAdminConfigurations {
    BrokerRestrictToLiquidationReasons restrict_to_liquidation_reasons;
    bool outgoing_transfers_blocked = false;
    bool incoming_transfers_blocked = false;
    bool disable_shorting = false;
    std::string pdt_check_mode;
    bool disable_fractional = false;
    bool disable_crypto = false;
    bool disable_day_trading = false;
    bool allow_instant_ach = false;
    bool disable_algodash_access = false;
    bool disable_api_key = false;
    BrokerAccountTradingDetailsValueChange max_margin_multiplier;
    BrokerAccountTradingDetailsValueChange max_options_trading_level;
    Decimal acct_daily_transfer_limit;

    BrokerAdminConfigurations() = default;
    BrokerAdminConfigurations(const json& j)
        : outgoing_transfers_blocked(parse_bool(j, "outgoing_transfers_blocked")),
          incoming_transfers_blocked(parse_bool(j, "incoming_transfers_blocked")),
          disable_shorting(parse_bool(j, "disable_shorting")),
          pdt_check_mode(parse_string(j, "pdt_check_mode")),
          disable_fractional(parse_bool(j, "disable_fractional")),
          disable_crypto(parse_bool(j, "disable_crypto")),
          disable_day_trading(parse_bool(j, "disable_day_trading")),
          allow_instant_ach(parse_bool(j, "allow_instant_ach")),
          disable_algodash_access(parse_bool(j, "disable_algodash_access")),
          disable_api_key(parse_bool(j, "disable_api_key")),
          acct_daily_transfer_limit(parse_decimal(j, "acct_daily_transfer_limit")) {
        if (j.contains("restrict_to_liquidation_reasons") && j["restrict_to_liquidation_reasons"].is_object()) {
            restrict_to_liquidation_reasons = BrokerRestrictToLiquidationReasons(j["restrict_to_liquidation_reasons"]);
        }
        if (j.contains("max_margin_multiplier") && j["max_margin_multiplier"].is_object()) {
            max_margin_multiplier = BrokerAccountTradingDetailsValueChange(j["max_margin_multiplier"]);
        }
        if (j.contains("max_options_trading_level") && j["max_options_trading_level"].is_object()) {
            max_options_trading_level = BrokerAccountTradingDetailsValueChange(j["max_options_trading_level"]);
        }
    }

    json to_json() const {
        json j;
        j["restrict_to_liquidation_reasons"] = restrict_to_liquidation_reasons.to_json();
        j["outgoing_transfers_blocked"] = outgoing_transfers_blocked;
        j["incoming_transfers_blocked"] = incoming_transfers_blocked;
        j["disable_shorting"] = disable_shorting;
        if (!pdt_check_mode.empty()) j["pdt_check_mode"] = pdt_check_mode;
        j["disable_fractional"] = disable_fractional;
        j["disable_crypto"] = disable_crypto;
        j["disable_day_trading"] = disable_day_trading;
        j["allow_instant_ach"] = allow_instant_ach;
        j["disable_algodash_access"] = disable_algodash_access;
        j["disable_api_key"] = disable_api_key;
        if (!max_margin_multiplier.empty()) j["max_margin_multiplier"] = max_margin_multiplier.to_json();
        if (!max_options_trading_level.empty()) j["max_options_trading_level"] = max_options_trading_level.to_json();
        j["acct_daily_transfer_limit"] = acct_daily_transfer_limit.to_string();
        return j;
    }
};

// BrokerAccountTradingDetails models GET /v1/trading/accounts/{account_id}/account.
// Alpaca documents this separately from GET /v1/accounts/{account_id}.
struct BrokerAccountTradingDetails {
    std::string id;
    BrokerAdminConfigurations admin_configurations;
    BrokerTradingConfigurations user_configurations;
    std::string account_number;
    std::string status;
    std::string crypto_status;
    std::string currency;
    Decimal buying_power;
    Decimal regt_buying_power;
    Decimal daytrading_buying_power;
    Decimal effective_buying_power;
    Decimal non_marginable_buying_power;
    Decimal options_buying_power;
    Decimal bod_dtbp;
    Decimal cash;
    Decimal cash_withdrawable;
    Decimal cash_transferable;
    Decimal pending_transfer_out;
    Decimal pending_transfer_in;
    Decimal accrued_fees;
    Decimal portfolio_value;
    bool pattern_day_trader = false;
    bool trading_blocked = false;
    bool transfers_blocked = false;
    bool account_blocked = false;
    DateTime created_at;
    bool trade_suspended_by_user = false;
    std::string multiplier;
    bool shorting_enabled = false;
    Decimal equity;
    Decimal last_equity;
    Decimal long_market_value;
    Decimal short_market_value;
    Decimal position_market_value;
    Decimal initial_margin;
    Decimal maintenance_margin;
    Decimal last_maintenance_margin;
    Decimal sma;
    int daytrade_count = 0;
    DateTime balance_asof;
    DateTime previous_close;
    Decimal last_long_market_value;
    Decimal last_short_market_value;
    Decimal last_cash;
    Decimal last_initial_margin;
    Decimal last_regt_buying_power;
    Decimal last_daytrading_buying_power;
    Decimal last_options_buying_power;
    Decimal last_buying_power;
    int last_daytrade_count = 0;
    std::string clearing_broker;
    int options_approved_level = 0;
    int options_trading_level = 0;
    Decimal intraday_adjustments;
    Decimal pending_reg_taf_fees;
    Decimal memoposts;

    BrokerAccountTradingDetails() = default;
    BrokerAccountTradingDetails(const json& j)
        : id(parse_string(j, "id")),
          account_number(parse_string(j, "account_number")),
          status(parse_string(j, "status")),
          crypto_status(parse_string(j, "crypto_status")),
          currency(parse_string(j, "currency")),
          buying_power(parse_decimal(j, "buying_power")),
          regt_buying_power(parse_decimal(j, "regt_buying_power")),
          daytrading_buying_power(parse_decimal(j, "daytrading_buying_power")),
          effective_buying_power(parse_decimal(j, "effective_buying_power")),
          non_marginable_buying_power(parse_decimal(j, "non_marginable_buying_power")),
          options_buying_power(parse_decimal(j, "options_buying_power")),
          bod_dtbp(parse_decimal(j, "bod_dtbp")),
          cash(parse_decimal(j, "cash")),
          cash_withdrawable(parse_decimal(j, "cash_withdrawable")),
          cash_transferable(parse_decimal(j, "cash_transferable")),
          pending_transfer_out(parse_decimal(j, "pending_transfer_out")),
          pending_transfer_in(parse_decimal(j, "pending_transfer_in")),
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
          balance_asof(parse_datetime(j, "balance_asof")),
          previous_close(parse_datetime(j, "previous_close")),
          last_long_market_value(parse_decimal(j, "last_long_market_value")),
          last_short_market_value(parse_decimal(j, "last_short_market_value")),
          last_cash(parse_decimal(j, "last_cash")),
          last_initial_margin(parse_decimal(j, "last_initial_margin")),
          last_regt_buying_power(parse_decimal(j, "last_regt_buying_power")),
          last_daytrading_buying_power(parse_decimal(j, "last_daytrading_buying_power")),
          last_options_buying_power(parse_decimal(j, "last_options_buying_power")),
          last_buying_power(parse_decimal(j, "last_buying_power")),
          last_daytrade_count(parse_int(j, "last_daytrade_count")),
          clearing_broker(parse_string(j, "clearing_broker")),
          options_approved_level(parse_int(j, "options_approved_level")),
          options_trading_level(parse_int(j, "options_trading_level")),
          intraday_adjustments(parse_decimal(j, "intraday_adjustments")),
          pending_reg_taf_fees(parse_decimal(j, "pending_reg_taf_fees")),
          memoposts(parse_decimal(j, "memoposts")) {
        if (j.contains("admin_configurations") && j["admin_configurations"].is_object()) {
            admin_configurations = BrokerAdminConfigurations(j["admin_configurations"]);
        }
        if (j.contains("user_configurations") && j["user_configurations"].is_object()) {
            user_configurations = BrokerTradingConfigurations(j["user_configurations"]);
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "BrokerAccountTradingDetails Information:\n"
            << "  ID: " << id << "\n"
            << "  Account Number: " << account_number << "\n"
            << "  Status: " << status << "\n"
            << "  Crypto Status: " << crypto_status << "\n"
            << "  Currency: " << currency << "\n"
            << "  Buying Power: " << buying_power << "\n"
            << "  Cash: " << cash << "\n"
            << "  Equity: " << equity << "\n"
            << "  Last Equity: " << last_equity << "\n"
            << "  Clearing Broker: " << clearing_broker << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["admin_configurations"] = admin_configurations.to_json();
        j["user_configurations"] = user_configurations.to_json();
        j["account_number"] = account_number;
        j["status"] = status;
        j["crypto_status"] = crypto_status;
        j["currency"] = currency;
        j["buying_power"] = buying_power.to_string();
        j["regt_buying_power"] = regt_buying_power.to_string();
        j["daytrading_buying_power"] = daytrading_buying_power.to_string();
        j["effective_buying_power"] = effective_buying_power.to_string();
        j["non_marginable_buying_power"] = non_marginable_buying_power.to_string();
        j["options_buying_power"] = options_buying_power.to_string();
        j["bod_dtbp"] = bod_dtbp.to_string();
        j["cash"] = cash.to_string();
        j["cash_withdrawable"] = cash_withdrawable.to_string();
        j["cash_transferable"] = cash_transferable.to_string();
        j["pending_transfer_out"] = pending_transfer_out.to_string();
        j["pending_transfer_in"] = pending_transfer_in.to_string();
        j["accrued_fees"] = accrued_fees.to_string();
        j["portfolio_value"] = portfolio_value.to_string();
        j["pattern_day_trader"] = pattern_day_trader;
        j["trading_blocked"] = trading_blocked;
        j["transfers_blocked"] = transfers_blocked;
        j["account_blocked"] = account_blocked;
        if (created_at.to_epoch_seconds() > 0) j["created_at"] = created_at.to_iso_string();
        j["trade_suspended_by_user"] = trade_suspended_by_user;
        j["multiplier"] = multiplier;
        j["shorting_enabled"] = shorting_enabled;
        j["equity"] = equity.to_string();
        j["last_equity"] = last_equity.to_string();
        j["long_market_value"] = long_market_value.to_string();
        j["short_market_value"] = short_market_value.to_string();
        j["position_market_value"] = position_market_value.to_string();
        j["initial_margin"] = initial_margin.to_string();
        j["maintenance_margin"] = maintenance_margin.to_string();
        j["last_maintenance_margin"] = last_maintenance_margin.to_string();
        j["sma"] = sma.to_string();
        j["daytrade_count"] = daytrade_count;
        if (balance_asof.to_epoch_seconds() > 0) j["balance_asof"] = balance_asof.to_iso_string();
        if (previous_close.to_epoch_seconds() > 0) j["previous_close"] = previous_close.to_iso_string();
        j["last_long_market_value"] = last_long_market_value.to_string();
        j["last_short_market_value"] = last_short_market_value.to_string();
        j["last_cash"] = last_cash.to_string();
        j["last_initial_margin"] = last_initial_margin.to_string();
        j["last_regt_buying_power"] = last_regt_buying_power.to_string();
        j["last_daytrading_buying_power"] = last_daytrading_buying_power.to_string();
        j["last_options_buying_power"] = last_options_buying_power.to_string();
        j["last_buying_power"] = last_buying_power.to_string();
        j["last_daytrade_count"] = last_daytrade_count;
        j["clearing_broker"] = clearing_broker;
        j["options_approved_level"] = options_approved_level;
        j["options_trading_level"] = options_trading_level;
        j["intraday_adjustments"] = intraday_adjustments.to_string();
        j["pending_reg_taf_fees"] = pending_reg_taf_fees.to_string();
        j["memoposts"] = memoposts.to_string();
        return j;
    }
};

} // namespace alpaca
