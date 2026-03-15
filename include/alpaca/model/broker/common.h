#pragma once

#include <string>
#include <sstream>

#include "nlohmann/json.hpp"

namespace alpaca {

using json = nlohmann::json;

// Generic JSON wrappers used by Broker API endpoints that do not yet have
// dedicated typed models in alpaca-cpp.
struct BrokerEntity {
    json value;

    BrokerEntity() = default;
    explicit BrokerEntity(const json& j) : value(j) {}

    json to_json() const { return value; }
    std::string to_string() const { return value.dump(2); }
};

struct BrokerSSESubscription {
    std::string stream_url;

    BrokerSSESubscription() = default;
    explicit BrokerSSESubscription(const std::string& url) : stream_url(url) {}

    std::string to_string() const {
        std::ostringstream oss;
        oss << "SSE stream URL: " << stream_url;
        return oss.str();
    }
};

} // namespace alpaca
