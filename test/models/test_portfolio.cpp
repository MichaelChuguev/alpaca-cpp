#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/portfolio.h"
#include "alpaca/core/types.h"

using json = nlohmann::json;
using namespace alpaca;

TEST(PortfolioSnapshotTest, ParseFromJson) {
    json j;
    j["timestamp"] = "2022-01-01T00:00:00Z";
    j["equity"] = 100000.0;
    j["profit_loss"] = 500.0;
    j["profit_loss_pct"] = 0.005;

    PortfolioSnapshot snapshot(j);

    EXPECT_EQ(snapshot.equity, Decimal(100000.0));
    EXPECT_EQ(snapshot.profit_loss, Decimal(500.0));
    EXPECT_EQ(snapshot.profit_loss_pct, Decimal(0.005));
}

TEST(PortfolioSnapshotTest, ToJson) {
    json j;
    j["timestamp"] = "2022-01-01T00:00:00Z";
    j["equity"] = 100000.0;
    j["profit_loss"] = 500.0;
    j["profit_loss_pct"] = 0.005;

    PortfolioSnapshot snapshot(j);
    json result = snapshot.to_json();

    EXPECT_DOUBLE_EQ(result["equity"].get<double>(), 100000.0);
    EXPECT_DOUBLE_EQ(result["profit_loss"].get<double>(), 500.0);
}

TEST(PortfolioSnapshotTest, ToString) {
    json j;
    j["timestamp"] = "2022-01-01T00:00:00Z";
    j["equity"] = 100000.0;
    j["profit_loss"] = 500.0;
    j["profit_loss_pct"] = 0.005;

    PortfolioSnapshot snapshot(j);
    std::string str = snapshot.to_string();

    EXPECT_NE(str.find("Portfolio Snapshot:"), std::string::npos);
}

TEST(PortfolioHistoryTest, ParseFromJson) {
    json j;
    j["timestamp"] = json::array({1640995200, 1641081600, 1641168000});
    j["equity"] = json::array({100000.0, 100500.0, 101000.0});
    j["profit_loss"] = json::array({0.0, 500.0, 1000.0});
    j["profit_loss_pct"] = json::array({0.0, 0.005, 0.01});
    j["base_value"] = 100000.0;
    j["base_value_asof"] = "2022-01-01";
    j["timeframe"] = "1D";

    PortfolioHistory history(j);

    EXPECT_EQ(history.snapshots.size(), 3u);
    EXPECT_EQ(history.base_value, Decimal(100000.0));
    EXPECT_EQ(history.timeframe, "1D");
    EXPECT_EQ(history.snapshots[1].equity, Decimal(100500.0));
    EXPECT_EQ(history.snapshots[2].profit_loss, Decimal(1000.0));
}

TEST(PortfolioHistoryTest, ToString) {
    json j;
    j["timestamp"] = json::array({1640995200});
    j["equity"] = json::array({100000.0});
    j["profit_loss"] = json::array({0.0});
    j["profit_loss_pct"] = json::array({0.0});
    j["base_value"] = 100000.0;
    j["base_value_asof"] = "2022-01-01";
    j["timeframe"] = "1D";

    PortfolioHistory history(j);
    std::string str = history.to_string();

    EXPECT_NE(str.find("Portfolio History:"), std::string::npos);
    EXPECT_NE(str.find("1D"), std::string::npos);
}
