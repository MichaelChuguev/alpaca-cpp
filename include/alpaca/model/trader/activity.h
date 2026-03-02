#pragma once

#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/core/util.h"

namespace alpaca {

using json = nlohmann::json;

struct BaseActivity {
    std::string account_activity_type;
    std::string activity_type;
    std::string id;
    
    virtual ~BaseActivity() = default;  // Virtual destructor for proper cleanup
    virtual std::string to_string() const = 0;  // Pure virtual function
};

struct AccountActivity : public BaseActivity {

    std::string symbol;
    Decimal qty;

    // Trade
    Decimal cum_qty;
    Decimal leaves_qty;
    Decimal price;
    std::string side;
    std::string transaction_time;
    std::string order_id;
    std::string type;
    std::string order_status;

    // Non-trade
    std::string activity_sub_type;
    DateTime date;
    Decimal net_amount;
    Decimal per_share_amount; // Only for dividend activities
    std::string description;
    std::string status;

    AccountActivity(const json& j) {
        
        if (j.contains("activity_type")) activity_type = j["activity_type"];
        if (j.contains("id")) id = j["id"];
        if (j.contains("symbol")) symbol = j["symbol"];
        
        qty = j.contains("qty") ? Decimal(j["qty"].get<std::string>()) : Decimal{};

        // Trade Activity
        if (j.contains("order_id") && j.contains("order_status") && j.contains("type") &&
            j.contains("cum_qty") && j.contains("leaves_qty") && j.contains("price") &&
            j.contains("qty") && j.contains("side") && j.contains("transaction_time")) {

            account_activity_type = "trade_activity";
            cum_qty = Decimal(j["cum_qty"].get<std::string>());
            leaves_qty = Decimal(j["leaves_qty"].get<std::string>());
            price = Decimal(j["price"].get<std::string>());
            side = j["side"];
            transaction_time = j["transaction_time"];
            order_id = j["order_id"];
            type = j["type"];
            order_status = j["order_status"];

        } else if (j.contains("date") && 
            j.contains("net_amount") && j.contains("description") && j.contains("status")) {
            
            account_activity_type = "non_trade_activity";
            if (j.contains("activity_sub_type")) activity_sub_type = j["activity_sub_type"].get<std::string>();
            date = DateTime::parse(j["date"].get<std::string>());
            net_amount = Decimal(j["net_amount"].get<std::string>());
            if (j.contains("per_share_amount")) per_share_amount = Decimal(j["per_share_amount"].get<std::string>());
            description = j["description"].get<std::string>();
            status = j["status"].get<std::string>();

        } else {
            account_activity_type = "other_activity"; // miscellaneous
        }
    }

    std::string to_string() const override {
        std::ostringstream oss;
        oss << "Account Activity:\n"
            << "Account Activity Type: " << account_activity_type << "\n"
            << "Activity Type: " << activity_type << "\n"
            << "ID: " << id << "\n";
        if (account_activity_type == "trade_activity") {
            oss << "Symbol: " << symbol << "\n"
                << "Cumulative Quantity: " << cum_qty << "\n"
                << "Leaves Quantity: " << leaves_qty << "\n"
                << "Price: " << price << "\n"
                << "Side: " << side << "\n"
                << "Transaction Time: " << transaction_time << "\n"
                << "Order ID: " << order_id << "\n"
                << "Type: " << type << "\n"
                << "Order Status: " << order_status << "\n";
        } else if (account_activity_type == "non_trade_activity") {
            oss << "Symbol: " << symbol << "\n"
                << "Activity Sub Type: " << activity_sub_type << "\n"
                << "Date: " << date.to_string() << "\n"
                << "Net Amount: " << net_amount << "\n"
                << "Per Share Amount: " << per_share_amount << "\n"
                << "Description: " << description << "\n"
                << "Status: " << status << "\n";
        } 
        
        return oss.str();
    }

    json to_json() const {
        json j;
        j["account_activity_type"] = account_activity_type;
        j["activity_type"] = activity_type;
        j["id"] = id;
        j["symbol"] = symbol;
        j["qty"] = qty.to_double();

        if (account_activity_type == "trade_activity") {
            j["cum_qty"] = cum_qty.to_double();
            j["leaves_qty"] = leaves_qty.to_double();
            j["price"] = price.to_double();
            j["side"] = side;
            j["transaction_time"] = transaction_time;
            j["order_id"] = order_id;
            j["type"] = type;
            j["order_status"] = order_status;
        } else if (account_activity_type == "non_trade_activity") {
            j["activity_sub_type"] = activity_sub_type;
            j["date"] = date.to_string();
            j["net_amount"] = net_amount.to_double();
            j["per_share_amount"] = per_share_amount.to_double();
            j["description"] = description;
            j["status"] = status;
        }

        return j;
    }

};


} // namespace alpaca