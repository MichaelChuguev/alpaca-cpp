#pragma once

#include <string>
#include <sstream>

#include "nlohmann/json.hpp"

#include "alpaca/core/types.h"

namespace alpaca {

using json = nlohmann::json;

struct Clock {

    DateTime timestamp;
    bool is_open;
    DateTime next_open;
    DateTime next_close;

    Clock(const json& j) 
    :
        timestamp(DateTime::parse(j["timestamp"])),
        is_open(j["is_open"].get<bool>()),
        next_open(DateTime::parse(j["next_open"])),
        next_close(DateTime::parse(j["next_close"]))
    {    
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Clock:\n"
            << "Timestamp: " << timestamp.to_string() << "\n"
            << "Is Open: " << is_open << "\n"
            << "Next Open: " << next_open.to_string() << "\n"
            << "Next Close: " << next_close.to_string() << "\n";
            
        return oss.str();
    }

    json to_json() const {
        json j;
        j["timestamp"] = timestamp.to_string();
        j["is_open"] = is_open;
        j["next_open"] = next_open.to_string();
        j["next_close"] = next_close.to_string();
        return j;
    }

};

} // namespace alpaca