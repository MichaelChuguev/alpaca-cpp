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
            std::runtime_error(j["message"].get<std::string>()),
            code_(j["code"].get<unsigned>())
        {
        }
    
        unsigned code() const noexcept { return code_; }

    private:
        unsigned code_;

        
    };



} // namespace alpaca