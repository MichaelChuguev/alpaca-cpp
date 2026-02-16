#pragma once

#include <string>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"
#include "alpaca/model/trader/asset.h"

namespace alpaca {

using json = nlohmann::json;

struct Watchlist {

    std::string id;
    std::string account_id;
    DateTime created_at;
    DateTime updated_at;
    std::string name;
    std::vector<Asset> assets;

    Watchlist(const json& j)
    : 
        id(j["id"].get<std::string>()),
        account_id(j["account_id"].get<std::string>()),
        created_at(DateTime::parse(j["created_at"])),
        updated_at(DateTime::parse(j["updated_at"])),
        name(j["name"].get<std::string>())
    {
        if (j.contains("assets") && j["assets"].is_array()) {
            for (const auto& assetJson : j["assets"]) {
                assets.emplace_back(assetJson);  
            }
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Watchlist:\n"
            << "ID: " << id << "\n"
            << "Account ID: " << account_id << "\n"
            << "Created At: " << created_at.to_string() << "\n"
            << "Updated At: " << updated_at.to_string() << "\n"
            << "Name: " << name << "\n"
            << "Assets {\n";
        for (const Asset& a : assets) {
            oss << a.to_string() << "\n"; 
        }
        oss << "}";
        return oss.str();
    }

    json to_json() const {
        json j;
        j["id"] = id;
        j["account_id"] = account_id;
        j["created_at"] = created_at.to_string();
        j["updated_at"] = updated_at.to_string();
        j["name"] = name;
        j["assets"] = json::array();
        for (const auto& a : assets) {
            j["assets"].push_back(a.to_json());
        }
        return j;
    }
};

} // namespace alpaca