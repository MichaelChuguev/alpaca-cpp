#pragma once

#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

struct PortfolioSnapshot {

    DateTime timestamp;
    double equity;
    double profit_loss;
    double profit_loss_pct;

    PortfolioSnapshot(const json& j)
    :
        timestamp(DateTime::parse(j["timestamp"])),
        equity(j["equity"].get<double>()),
        profit_loss(j["profit_loss"].get<double>()),
        profit_loss_pct(j["profit_loss_pct"].get<double>())
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Portfolio Snapshot:\n"
            << "Timestamp: " << timestamp.to_string() << "\n"
            << "Equity: " << equity << "\n"
            << "Profit/Loss: " << profit_loss << "\n"
            << "Profit/Loss Percent: " << profit_loss_pct << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["timestamp"] = timestamp.to_string();
        j["equity"] = equity;
        j["profit_loss"] = profit_loss;
        j["profit_loss_pct"] = profit_loss_pct;
        return j;
    }

};

struct PortfolioHistory {
    
    std::vector<PortfolioSnapshot> snapshots;
    double base_value;
    DateTime base_value_asof;
    std::string timeframe;
    // Not in API response -> Object cashflow

    PortfolioHistory(const json& j)
    :
        base_value(j["base_value"].get<double>()),
        base_value_asof(DateTime::parse(j["base_value_asof"])),
        timeframe(j["timeframe"].get<std::string>())
    {
        const auto& timestamps = j["timestamp"];
        const auto& equities = j["equity"];
        const auto& profit_losses = j["profit_loss"];
        const auto& profit_loss_pcts = j["profit_loss_pct"];

        for (size_t i = 0; i < timestamps.size(); ++i) {
            snapshots.emplace_back(json{
                {"timestamp", timestamps[i]},
                {"equity", equities[i]},
                {"profit_loss", profit_losses[i]},
                {"profit_loss_pct", profit_loss_pcts[i]}
            });
        }
    }

    std::string to_string() {
        std::ostringstream oss;
        oss << "Portfolio History:\n"
            << "Snapshots: {\n";
            for (const auto& ss : snapshots) oss << ss.to_string() << "\n";
        oss << "}\n"
            << "Base Value: " << base_value << "\n"
            << "Base Value As Of: " << base_value_asof.to_string() << "\n"
            << "Timeframe: " << timeframe << "\n";

        return oss.str();
    }

};

enum IntradayReporting {
    MARKET_HOURS,
    EXTENDED_HOURS,
    CONTINUOUS
};

enum PnlReset{
    PER_DAY,
    NO_RESET
};


} // namespace alpaca