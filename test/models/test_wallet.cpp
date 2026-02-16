#include <gtest/gtest.h>
#include "nlohmann/json.hpp"
#include "alpaca/model/trader/wallet.h"

using json = nlohmann::json;
using namespace alpaca;

// ===== CryptoWallet Tests =====

TEST(CryptoWalletTest, ParseFromJson) {
    json j;
    j["id"] = "wallet-123";
    j["asset"] = "BTC";
    j["qty"] = "1.5";
    j["available_qty"] = "1.0";
    j["pending_qty"] = "0.3";
    j["locked_qty"] = "0.2";

    CryptoWallet wallet(j);

    EXPECT_EQ(wallet.id, "wallet-123");
    EXPECT_EQ(wallet.asset, "BTC");
    EXPECT_DOUBLE_EQ(wallet.qty, 1.5);
    EXPECT_DOUBLE_EQ(wallet.available_qty, 1.0);
    EXPECT_DOUBLE_EQ(wallet.pending_qty, 0.3);
    EXPECT_DOUBLE_EQ(wallet.locked_qty, 0.2);
}

TEST(CryptoWalletTest, ParseMissingFields) {
    json j;
    j["id"] = "wallet-123";
    j["asset"] = "ETH";

    CryptoWallet wallet(j);

    EXPECT_EQ(wallet.id, "wallet-123");
    EXPECT_EQ(wallet.asset, "ETH");
    EXPECT_DOUBLE_EQ(wallet.qty, 0.0);
    EXPECT_DOUBLE_EQ(wallet.available_qty, 0.0);
}

TEST(CryptoWalletTest, ToJson) {
    json j;
    j["id"] = "wallet-123";
    j["asset"] = "BTC";
    j["qty"] = "1.5";
    j["available_qty"] = "1.0";
    j["pending_qty"] = "0.3";
    j["locked_qty"] = "0.2";

    CryptoWallet wallet(j);
    json result = wallet.to_json();

    EXPECT_EQ(result["id"], "wallet-123");
    EXPECT_EQ(result["asset"], "BTC");
}

TEST(CryptoWalletTest, ToString) {
    json j;
    j["id"] = "wallet-123";
    j["asset"] = "BTC";
    j["qty"] = "1.5";
    j["available_qty"] = "1.0";
    j["pending_qty"] = "0.0";
    j["locked_qty"] = "0.0";

    CryptoWallet wallet(j);
    std::string str = wallet.to_string();

    EXPECT_NE(str.find("Crypto Wallet:"), std::string::npos);
    EXPECT_NE(str.find("BTC"), std::string::npos);
}

TEST(CryptoWalletTest, ParseFromJsonArray) {
    json arr = json::array();
    json btc;
    btc["id"] = "w1";
    btc["asset"] = "BTC";
    btc["qty"] = "1.0";
    btc["available_qty"] = "1.0";
    btc["pending_qty"] = "0.0";
    btc["locked_qty"] = "0.0";

    json eth;
    eth["id"] = "w2";
    eth["asset"] = "ETH";
    eth["qty"] = "10.0";
    eth["available_qty"] = "10.0";
    eth["pending_qty"] = "0.0";
    eth["locked_qty"] = "0.0";

    arr.push_back(btc);
    arr.push_back(eth);

    std::vector<CryptoWallet> wallets;
    for (const auto& w : arr) {
        wallets.push_back(CryptoWallet(w));
    }

    ASSERT_EQ(wallets.size(), 2u);
    EXPECT_EQ(wallets[0].asset, "BTC");
    EXPECT_EQ(wallets[1].asset, "ETH");
}

// ===== CryptoTransfer Tests =====

TEST(CryptoTransferTest, ParseFromJson) {
    json j;
    j["id"] = "transfer-456";
    j["tx_hash"] = "0xabc123";
    j["direction"] = "OUTGOING";
    j["status"] = "completed";
    j["amount"] = "0.5";
    j["network"] = "bitcoin";
    j["fees"] = "0.0001";
    j["chain"] = "BTC";
    j["asset"] = "BTC";
    j["created_at"] = "2022-06-01T12:00:00Z";
    j["updated_at"] = "2022-06-01T12:05:00Z";
    j["expires_at"] = "2022-06-02T12:00:00Z";
    j["wallet_id"] = "wallet-123";
    j["address"] = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";

    CryptoTransfer transfer(j);

    EXPECT_EQ(transfer.id, "transfer-456");
    EXPECT_EQ(transfer.tx_hash, "0xabc123");
    EXPECT_EQ(transfer.direction, "OUTGOING");
    EXPECT_EQ(transfer.status, "completed");
    EXPECT_DOUBLE_EQ(transfer.amount, 0.5);
    EXPECT_EQ(transfer.network, "bitcoin");
    EXPECT_DOUBLE_EQ(transfer.fees, 0.0001);
    EXPECT_EQ(transfer.chain, "BTC");
    EXPECT_EQ(transfer.asset, "BTC");
    EXPECT_EQ(transfer.wallet_id, "wallet-123");
    EXPECT_EQ(transfer.address, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa");
}

TEST(CryptoTransferTest, ParseMissingOptionalFields) {
    json j;
    j["id"] = "transfer-456";
    j["status"] = "pending";
    j["amount"] = "1.0";

    CryptoTransfer transfer(j);

    EXPECT_EQ(transfer.id, "transfer-456");
    EXPECT_EQ(transfer.status, "pending");
    EXPECT_DOUBLE_EQ(transfer.amount, 1.0);
    EXPECT_EQ(transfer.tx_hash, "");
}

TEST(CryptoTransferTest, ToJson) {
    json j;
    j["id"] = "transfer-456";
    j["tx_hash"] = "0xabc123";
    j["direction"] = "OUTGOING";
    j["status"] = "completed";
    j["amount"] = "0.5";
    j["network"] = "bitcoin";
    j["fees"] = "0.0001";
    j["chain"] = "BTC";
    j["asset"] = "BTC";
    j["created_at"] = "2022-06-01T12:00:00Z";
    j["updated_at"] = "2022-06-01T12:05:00Z";
    j["expires_at"] = "2022-06-02T12:00:00Z";
    j["wallet_id"] = "wallet-123";
    j["address"] = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";

    CryptoTransfer transfer(j);
    json result = transfer.to_json();

    EXPECT_EQ(result["id"], "transfer-456");
    EXPECT_EQ(result["direction"], "OUTGOING");
    EXPECT_EQ(result["asset"], "BTC");
}

TEST(CryptoTransferTest, ToString) {
    json j;
    j["id"] = "transfer-456";
    j["tx_hash"] = "0xabc123";
    j["direction"] = "OUTGOING";
    j["status"] = "completed";
    j["amount"] = "0.5";
    j["network"] = "bitcoin";
    j["fees"] = "0.0001";
    j["chain"] = "BTC";
    j["asset"] = "BTC";
    j["created_at"] = "2022-06-01T12:00:00Z";
    j["updated_at"] = "2022-06-01T12:05:00Z";
    j["expires_at"] = "2022-06-02T12:00:00Z";
    j["wallet_id"] = "wallet-123";
    j["address"] = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";

    CryptoTransfer transfer(j);
    std::string str = transfer.to_string();

    EXPECT_NE(str.find("Crypto Transfer:"), std::string::npos);
    EXPECT_NE(str.find("OUTGOING"), std::string::npos);
}

// ===== CryptoTransferRequest Tests =====

TEST(CryptoTransferRequestTest, ConstructAndToJson) {
    CryptoTransferRequest req(0.5, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa", "BTC", "bitcoin");
    json result = req.to_json();

    EXPECT_EQ(result["address"], "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa");
    EXPECT_EQ(result["asset"], "BTC");
    EXPECT_EQ(result["network"], "bitcoin");
}

TEST(CryptoTransferRequestTest, DefaultConstruct) {
    CryptoTransferRequest req;
    req.amount = 1.0;
    req.address = "addr";
    req.asset = "ETH";
    req.network = "ethereum";

    json result = req.to_json();
    EXPECT_EQ(result["asset"], "ETH");
    EXPECT_EQ(result["network"], "ethereum");
}

// ===== WhitelistedAddress Tests =====

TEST(WhitelistedAddressTest, ParseFromJson) {
    json j;
    j["id"] = "wl-addr-1";
    j["asset"] = "BTC";
    j["address"] = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    j["status"] = "active";
    j["network"] = "bitcoin";
    j["created_at"] = "2022-06-01T12:00:00Z";

    WhitelistedAddress addr(j);

    EXPECT_EQ(addr.id, "wl-addr-1");
    EXPECT_EQ(addr.asset, "BTC");
    EXPECT_EQ(addr.address, "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa");
    EXPECT_EQ(addr.status, "active");
    EXPECT_EQ(addr.network, "bitcoin");
}

TEST(WhitelistedAddressTest, ParseMissingFields) {
    json j;
    j["id"] = "wl-addr-1";
    j["asset"] = "BTC";

    WhitelistedAddress addr(j);

    EXPECT_EQ(addr.id, "wl-addr-1");
    EXPECT_EQ(addr.asset, "BTC");
    EXPECT_EQ(addr.address, "");
}

TEST(WhitelistedAddressTest, ToJson) {
    json j;
    j["id"] = "wl-addr-1";
    j["asset"] = "BTC";
    j["address"] = "addr123";
    j["status"] = "active";
    j["network"] = "bitcoin";
    j["created_at"] = "2022-06-01T12:00:00Z";

    WhitelistedAddress addr(j);
    json result = addr.to_json();

    EXPECT_EQ(result["id"], "wl-addr-1");
    EXPECT_EQ(result["asset"], "BTC");
}

TEST(WhitelistedAddressTest, ToString) {
    json j;
    j["id"] = "wl-addr-1";
    j["asset"] = "BTC";
    j["address"] = "addr123";
    j["status"] = "active";
    j["network"] = "bitcoin";
    j["created_at"] = "2022-06-01T12:00:00Z";

    WhitelistedAddress addr(j);
    std::string str = addr.to_string();

    EXPECT_NE(str.find("Whitelisted Address:"), std::string::npos);
    EXPECT_NE(str.find("BTC"), std::string::npos);
}

// ===== WhitelistedAddressRequest Tests =====

TEST(WhitelistedAddressRequestTest, ConstructAndToJson) {
    WhitelistedAddressRequest req("addr123", "BTC");
    json result = req.to_json();

    EXPECT_EQ(result["address"], "addr123");
    EXPECT_EQ(result["asset"], "BTC");
}

// ===== CryptoTransferEstimate Tests =====

TEST(CryptoTransferEstimateTest, ParseFromJson) {
    json j;
    j["fee"] = "0.001";
    j["asset"] = "BTC";
    j["network"] = "bitcoin";

    CryptoTransferEstimate est(j);

    EXPECT_DOUBLE_EQ(est.fee, 0.001);
    EXPECT_EQ(est.asset, "BTC");
    EXPECT_EQ(est.network, "bitcoin");
}

TEST(CryptoTransferEstimateTest, ParseMissingFee) {
    json j;
    j["asset"] = "ETH";
    j["network"] = "ethereum";

    CryptoTransferEstimate est(j);

    EXPECT_DOUBLE_EQ(est.fee, 0.0);
    EXPECT_EQ(est.asset, "ETH");
}

TEST(CryptoTransferEstimateTest, ToJson) {
    json j;
    j["fee"] = "0.001";
    j["asset"] = "BTC";
    j["network"] = "bitcoin";

    CryptoTransferEstimate est(j);
    json result = est.to_json();

    EXPECT_EQ(result["asset"], "BTC");
    EXPECT_EQ(result["network"], "bitcoin");
}

TEST(CryptoTransferEstimateTest, ToString) {
    json j;
    j["fee"] = "0.001";
    j["asset"] = "BTC";
    j["network"] = "bitcoin";

    CryptoTransferEstimate est(j);
    std::string str = est.to_string();

    EXPECT_NE(str.find("Crypto Transfer Estimate:"), std::string::npos);
    EXPECT_NE(str.find("BTC"), std::string::npos);
}
