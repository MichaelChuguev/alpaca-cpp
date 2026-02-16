#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

struct CryptoWallet {

    std::string id;
    std::string asset;
    double qty;
    double available_qty;
    double pending_qty;
    double locked_qty;

    CryptoWallet(const json& j)
    :
        id(j.value("id", "")),
        asset(j.value("asset", "")),
        qty(j.contains("qty") ? std::stod(j["qty"].get<std::string>()) : 0.0),
        available_qty(j.contains("available_qty") ? std::stod(j["available_qty"].get<std::string>()) : 0.0),
        pending_qty(j.contains("pending_qty") ? std::stod(j["pending_qty"].get<std::string>()) : 0.0),
        locked_qty(j.contains("locked_qty") ? std::stod(j["locked_qty"].get<std::string>()) : 0.0)
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Crypto Wallet:\n"
            << "ID: " << id << "\n"
            << "Asset: " << asset << "\n"
            << "Qty: " << qty << "\n"
            << "Available Qty: " << available_qty << "\n"
            << "Pending Qty: " << pending_qty << "\n"
            << "Locked Qty: " << locked_qty << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["asset"] = asset;
        j["qty"] = std::to_string(qty);
        j["available_qty"] = std::to_string(available_qty);
        j["pending_qty"] = std::to_string(pending_qty);
        j["locked_qty"] = std::to_string(locked_qty);
        return j;
    }
};

struct CryptoTransfer {

    std::string id;
    std::string tx_hash;
    std::string direction;  // INCOMING or OUTGOING
    std::string status;
    double amount;
    std::string network;
    double fees;
    std::string chain;
    std::string asset;
    DateTime created_at;
    DateTime updated_at;
    DateTime expires_at;
    std::string wallet_id;
    std::string address;

    CryptoTransfer(const json& j)
    :
        id(j.value("id", "")),
        tx_hash(j.value("tx_hash", "")),
        direction(j.value("direction", "")),
        status(j.value("status", "")),
        amount(j.contains("amount") ? std::stod(j["amount"].get<std::string>()) : 0.0),
        network(j.value("network", "")),
        fees(j.contains("fees") && !j["fees"].is_null() ? std::stod(j["fees"].get<std::string>()) : 0.0),
        chain(j.value("chain", "")),
        asset(j.value("asset", "")),
        created_at(j.contains("created_at") && !j["created_at"].is_null() ? DateTime::parse(j["created_at"]) : DateTime()),
        updated_at(j.contains("updated_at") && !j["updated_at"].is_null() ? DateTime::parse(j["updated_at"]) : DateTime()),
        expires_at(j.contains("expires_at") && !j["expires_at"].is_null() ? DateTime::parse(j["expires_at"]) : DateTime()),
        wallet_id(j.value("wallet_id", "")),
        address(j.value("address", ""))
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Crypto Transfer:\n"
            << "ID: " << id << "\n"
            << "TX Hash: " << tx_hash << "\n"
            << "Direction: " << direction << "\n"
            << "Status: " << status << "\n"
            << "Amount: " << amount << "\n"
            << "Network: " << network << "\n"
            << "Fees: " << fees << "\n"
            << "Chain: " << chain << "\n"
            << "Asset: " << asset << "\n"
            << "Created At: " << created_at.to_string() << "\n"
            << "Updated At: " << updated_at.to_string() << "\n"
            << "Expires At: " << expires_at.to_string() << "\n"
            << "Wallet ID: " << wallet_id << "\n"
            << "Address: " << address << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["tx_hash"] = tx_hash;
        j["direction"] = direction;
        j["status"] = status;
        j["amount"] = std::to_string(amount);
        j["network"] = network;
        j["fees"] = std::to_string(fees);
        j["chain"] = chain;
        j["asset"] = asset;
        j["created_at"] = created_at.to_string();
        j["updated_at"] = updated_at.to_string();
        j["expires_at"] = expires_at.to_string();
        j["wallet_id"] = wallet_id;
        j["address"] = address;
        return j;
    }
};

struct CryptoTransferRequest {

    double amount;
    std::string address;
    std::string asset;
    std::string network;

    CryptoTransferRequest() = default;

    CryptoTransferRequest(const double& amount, const std::string& address, const std::string& asset, const std::string& network)
    :
        amount(amount),
        address(address),
        asset(asset),
        network(network)
    {
    }

    json to_json() const {
        json j;
        j["amount"] = std::to_string(amount);
        j["address"] = address;
        j["asset"] = asset;
        j["network"] = network;
        return j;
    }
};

struct WhitelistedAddress {

    std::string id;
    std::string asset;
    std::string address;
    std::string status;
    std::string network;
    DateTime created_at;

    WhitelistedAddress() = default;

    WhitelistedAddress(const json& j)
    :
        id(j.value("id", "")),
        asset(j.value("asset", "")),
        address(j.value("address", "")),
        status(j.value("status", "")),
        network(j.value("network", "")),
        created_at(j.contains("created_at") && !j["created_at"].is_null() ? DateTime::parse(j["created_at"]) : DateTime())
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Whitelisted Address:\n"
            << "ID: " << id << "\n"
            << "Asset: " << asset << "\n"
            << "Address: " << address << "\n"
            << "Status: " << status << "\n"
            << "Network: " << network << "\n"
            << "Created At: " << created_at.to_string() << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["asset"] = asset;
        j["address"] = address;
        j["status"] = status;
        j["network"] = network;
        j["created_at"] = created_at.to_string();
        return j;
    }
};

struct WhitelistedAddressRequest {

    std::string address;
    std::string asset;

    WhitelistedAddressRequest() = default;

    WhitelistedAddressRequest(const std::string& address, const std::string& asset)
    :
        address(address),
        asset(asset)
    {
    }

    json to_json() const {
        json j;
        j["address"] = address;
        j["asset"] = asset;
        return j;
    }
};

struct CryptoTransferEstimate {

    double fee;
    std::string asset;
    std::string network;

    CryptoTransferEstimate(const json& j)
    :
        fee(j.contains("fee") ? std::stod(j["fee"].get<std::string>()) : 0.0),
        asset(j.value("asset", "")),
        network(j.value("network", ""))
    {
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Crypto Transfer Estimate:\n"
            << "Fee: " << fee << "\n"
            << "Asset: " << asset << "\n"
            << "Network: " << network << "\n";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["fee"] = std::to_string(fee);
        j["asset"] = asset;
        j["network"] = network;
        return j;
    }
};

} // namespace alpaca
