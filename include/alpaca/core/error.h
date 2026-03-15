#pragma once

#include <string>
#include <exception>

#include "nlohmann/json.hpp"

namespace alpaca {

using json = nlohmann::json;

class AlpacaTraderException : public std::runtime_error {
    public:
       
        AlpacaTraderException(const json& j)
        :
            std::runtime_error(
                j.contains("message") && !j["message"].is_null()
                    ? j["message"].get<std::string>()
                    : "Unknown API error"),
            code_(j.contains("code") && !j["code"].is_null()
                    ? j["code"].get<unsigned>()
                    : 0)
        {
        }
    
        [[nodiscard]] unsigned code() const noexcept { return code_; }

    private:
        unsigned code_;
    };



} // namespace alpaca